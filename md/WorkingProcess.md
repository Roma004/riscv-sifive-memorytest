# Заметки о написании проекта

## Подготовительная работа (настройка окружения, написание базовых драйверов)

Первая либа [эта](https://github.com/sifive/freedom-metal/tree/1cec4a23a7ed7350db79a392be65acd51acd5412), что гуглится и засирает половину первой страницы поиска написана неочевидно, так что, её, пропустил сразу практически. Примеров кода подцепить от туда не удалось, как и каких-либо концепций.

Из того, что на первых порах нашёл в пойске была вот [эта](https://github.com/diodesign/mmio_sifive_uart/blob/main/src/lib.rs) либа на расте. Там очень незаморочено реализован минимальный драйвер UART модуля. Это код подтвердил мои предположения, что ничего, кроме записей по различным адресам мне в этом проекте делать не надо будет.

В поисках кросс-компилятора сразу загуглил, как поставить уже собранный на archlinux, но когда увидел, что из себя найденный AUR пакет представляет, решил, что будет правильнее самостоятельно скачать [этот](https://github.com/riscv-collab/riscv-gnu-toolchain/releases/tag/2023.07.07) nightly релиз под убунту, и положить куда мне надо.

Я переписал код с раста на С, но он не заработал (я бы больше удивился, противоположному результату). Несколько запусков кода, не показывающего никаких признаков жизни, но компилирующегося показали необходимость либо искать гайд по настройке окружения, либо конкретно дебаггера.

В результате усиленного гугления наткнулся на [этот](https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html) цикл статей. После вдумчивого прочтения многое встало на свои места. Я настроил отладчик, линкер, файлик с точкой вхождения кода (который перед вызовом main переставляет sp в начало стека), и перепроверил весь С код, написанный ранее.

Код начал подавать признаки жизни, но выводил строку не так, как предполагалось (байты дублировались, накладывались друг на друга, выводились не все и т.д.). После нескольких попыток понять, что не так, пришёл к выводу, что надо запускать с флагом -O0. Так же, поменял размер буффера, из которого я пишу символы в UART (поставил 2 байта). Оно стало работать нормально, но чудило при включении оптимизаций. Эту проблему решил отложить на потом

Ещё без флага компиляции -mabi=lp64 у меня дебаггер не хотел начинать отлаживать код из-за несовместимого набора инструкций (как я понял). А без флага `-mcmodel=medany` происходила какая-то фигня, и компилятор выдавал ошибку, что ему не то места не хватает на стеке, не то, ещё что-то. Но на гитхабе где-то нашёл, ответ что этот флаг иногда помогает

Теперь пришло время гуглить, что такое DMA, и как с ним работать. По-сути, это чип, который умеет перекладывать байты буфферами с одного места на другое без участия процессора. В общем, мне надо memcpy реализовать через API DMA. Дальше гайдами не пользовался, так как не нашёл их. Прочитал офф. доку по DMA для `fu540-c000-pdma` Очень помогло понимане концепции работы с различными устройствами в упомянутом выше цикле статей по настройке окружения.

Исходя из доки, всё, что мне надо - зарезервировать канал, записать регистры (memory-mapped) правильным образом -- передать настройки трансфера пакетов, кол-во байт и адреса откуда и куда писать. В течении нескольких часов реализовал базовый драйвер, исходя из написанного в доке, но он не завёлся. Ещё около 5 часов отладки привели меня к довольно полезной ссылке на [реализацию](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/dma/sf-pdma/sf-pdma.c?h=v6.3.12) этого драйвера под линь (вот ещё [заголовок](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/dma/sf-pdma/sf-pdma.h?h=v6.3.12)), и к некой адаптации в GDB (я вспомнил, как это делается спустя полтора года). Только не помогло решить проблему.

Без помощи научрука не удалось найти довольно неочевидный, на первый взгляд, косяк, что в пункте доки про расположение регистров в одну таблицу написали сдвиг каналов для обработки прерываний и сдвиг регистров конфигурации этих каналов. Можно было подумать, что сдвиг 0x8_0000 (каналов, на самом деле) - сдвиг регистров, а оказалось, всё наоборот. Пока я не перечитал раздел 12.2 вплоть до каждого слова, я даже бы не подумал об этом.

В общем, после решения этого, DMA драйвер завёлся, и мне удалось скопировать строку со стека на стек. Теперь привожу код в порядок, и иду думать, как лучше организовывать запись шаблона в оперативу.

На этом моменте появился более-менее сформировавшийся гит репозиторий (и этот док в месте с ним). так что, дальше буду прикладывать важные коммиты. [Коммит этого этапа](https://github.com/Roma004/riscv-shiva-memorytest/commit/894009e59ba2a8b6334743521ac26695298d0e49)

Переписал полностью интерфейс dma и его реализацию. Доработал структуры, используя bitfield. Довольно удобная штука, если в структуре надо задавать поля, расположенные с определённым сдвигом в битах

В результате многочисленных проверок я пришёл к выводу, что никак иначе, кроме как оборачиванием функций, занимающихся прямой записью в память в блок с нулевым уровнем оптимизаций я не заставлю этот код компилироваться правильно. Даже при наличии volatile у всех указателей компилятор что-то оптимизирует, и всё ломается. В uart выводятся дублированные сообщения. почему так происходит - разберусь потом. Но меня радует, что ситуация стала лучше. Как-минимум, dma работает полностью верно. [Коммит этого этапа](https://github.com/Roma004/riscv-shiva-memorytest/commit/5062a1450b0e9033376cbb4d6e8931f79a8bdbb4)

уже на этапе реализвции поставленной задачи пришло понимание, почему у меня постоянные проблемы с линковкой memcpy и memset, которые добавляются компилятором в качесиве оптимизаций по-умолчанию. В общем, проблема компилятора звучит так: `can't link double-float modules with soft-float modules`. Проблема в том, что я скопировал эти флаги компиляции из гайда, описанного в самом нечале, где автор не дал пояснений касательно того, почему они нужны. а нужны они - чтобы указать конфигурацию стандартных типов. Так вот я изначально написал `-mabi=lp64d` что означает, что я говорю записывать в f регистры числа с плавающей точкой двойной точности (double). Только у выбранной мной машины отсутствует FPU сопроцессор, а значит, я, и мой компилятор в том числе, в принципе не должен использовать операции с числами с плавающей точкой. Значит, мне таки надо собрать gcc самостоятельно

## Размышления о поставленной задаче

Задание было поставлено: проверить записанную через dma память при помощи процессора, и вывести результат в uart. Звучит, как-будто я должен использовать какие-то определённые api CPU, но, судя по всему нет. По-этому, всё, что мне надо - скопировать "строку" определённого формата в оперативу N раз, а потом циклом пробежаться, и провериить, что всё копируется правильно.

Если это действительно так, то на этапе подготовки было реализовано всё необходимое для этого.

Если я задание не понял, в любом случае, большая часть необходимого функционала готова, и переделать/доделать будет довольно легко.

Во время поисков на формах по похожим задачам нашёл для себя идею записывать в память байты типа `0b01010101` aka `ox55` или `0b10101010` aka `0xAA`. Смыл в том, что такой паттерн проверять проще всего. Можно читать области по 8 байт (64 битная архитектура позволяет делать это в одной командой ld) и битово умножать на свою инвертированную версию. Можно по-сути, использовать и другой паттерн, но этот задаётся, как мне кажется, наиболее очевидно.

Таким образом, план такой:

* объявляю в коде строку из 256 символов `'U'` aka `0x55`.
* из всех доступных мне 128M RAM оставлю у макушки стека 4К -- этого должно хватить с лихвой, не так уж и много данных у меня на стеке хранится.
* считаю оставшийся размер опреативы, делю на 4 области. На каждую из областей назначаю свой канал dma.
* в цикле проверяю: если трансфер на данном канале закончился - запустить канал на следующий адрес. Вместо того, чтобы просто ждать завершения трансфера на одном канале, продолжаю пробовать давать таску остальным.
* дополнительно реализую альтернативную функцию, где я буду использовать только 1 канал dma, чтобы сравнить скорость исполнения, и результат (если он, конечно, будет различаться)
* если при записи произошла ошибка (читаю бит error в control регистре потока) - тоже сохраняю инфу об этом
* после этого пробегаюсь циклом по всем адресам в опреативе, кроме тех 4К зарезервированных, проверяю их битовым умножением на `0xAA`. В результате должен получаться 0. если не получается - сохраняю информацию об этом
* вывожу итоговый отчёт

В принципе, как я понял из комментов различных мэтров микроэлектронники (или представляющих себя таковыми), если делать тест опреативной памяти, то надо обязательно записать паттерн в опреативу как можно быстрее. Судя по всему цель - записать все области в приблизительно одинаковых обстоятельствах, так как, по всей видимости, иногда результаты этого теста могут зависеть от напряжения сети, расположения Юпитера и прочих внешних параметров. Всю оперативу разом записать не получится, но надо свести к минимуму временные затраты, чтобы отделить проблемы нерабочих/невключенных чипов от других, менее зависящих от конкретно данной схемы параметров, влияющих на тест, повлиять на которые - задача менее тривиальная.

Для этого я и хочу попробовать 2 вида заполенния памяти. Чтобы была возможность, условно, последовательного и ассинхронного её заполнения. Вероятно, это даст в некотором роде, больше информации человеку, тестирующему опреативу.

## Заметки о реализации поставленной задачи

Получилось реализовать минимально рабочую версию алгоритма. Решил вместо двух функций, описанных в моём плане реализовать одну, в которой можно регулировать кол-во задействуемых каналов. В принципе, так и код не дублируется, и, это удобнее, и, в принципе, логичнее с учётом кода который написан.

теперь пришло время подумать о метриках, которые я буду собирать. В этом вопросе всё чуть помуторнее, ведь я пишу на С, а не на питоне. Так что, придётся прописать миллиард и одну структуру, каждая из которых будет описывать метрики.

Самое сложное в этой задаче - сделать удобной структуру отчёта. Никуда, кроме как в uart я не смогу вывести эти метрики, а значит, результат должен быть максимально сжат по кол-ву символов, при максимальной возможной информативности.

На данном этапе я могу отловить такие события

* dma не смог записать данный чанк
* данные 8 байт не соответствуют паттерну

Было бы довольно просто вывести их в uart, но тут вопрос в группировке данных. Так как оперативы может быть гигабайт (на пример), и четверть не записалась через dma, или половина не перезаписалась. Не выводить же 128к строк о том, что данные 8 байт неисправны. Тут надо скорее писать диапазон адресов, мол 0x80000000-0x80001000 не работают.

В голове были мысли, мол, а что делать, если каждый второй чанк не записался. Думал забить, но, как вариант не самый плохой, просто составить карту чанков, и выводить её большим массивом через плюсики и минусики. Можно разделять строку по 64 символа, и адрес подписывать, чтобы проще было ориентироваться. Это и реализуется через обычный массив