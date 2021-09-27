# На текучей реализации, что надо:

# Samples ZIP

> Собираем DSHost.
>> - О том как собирать, написано в DSHost README.
>
> Далее мы создаем папку Samples. И создаем подпапки:
>> - cache
>> - components
>> - config
>> - exe
>
> В папку "cache" надо докинуть либы:
>> - libDSUtils.[so|dll]
>
> В папку "exe" закидываем:
>> - dshost[.exe]
>
> После делаем архив:
>> - Samples.zip
>
> ### В папке bin создаем папку Samples, создаем под папки Linux WinNT и закидываем Samples.zip в соотвествии с ОС.

# macchina.io

> Путь расположения macchina, на:
> ### Linux
>> /usr/local/macchina
> ### Windows
>> C:/macchina

# ERROR

> ### Если DSHost выбивает ошибку под Windows:
>> MqttClient:cann't connect to: [ip], error: 14
>
> В файле "mosquitto.conf" по пути "C:\Program Files\mosquitto", надо добавить 2 строки:
>> - listener 1883
>> - allow_anonymous true

> ### Если возникает ошибка в Linux:
>> error while loading shared libraries: libPocoXMLd.so.64: cannot open shared object file: No such file or directory
>
> Надо выполнить:
>> sudo ldconfig /usr/local/macchina/lib/

> ### Если при запуске DSConstrol под Windows выбивает ошибку:
>> System Error: Программа не может найти Qt библиотеку "qt[Name].dll"
>
> Выполнить команду в терминале:
>> $ C:\\Qt\\5.15.2\\msvc2019_64\\bin\\windeployqt.exe DSControl.exe
>
> ### или
>> System Error: Программа не может найти Poco библиотеку "Poco[Name].dll"
>
> Надо скопировать с папки C:\\macchina\\bin все *.dll, и закинуть все в bin папку.

## Деплой

> Что бы сделать деплой, надо:
>> - Создаем папку bndl в папке bin
>> - Перенносим туда бандлы *_1.0.0.bndl, которые собрали на стороне DSHost
>
> Накидываем проект, нажимаем деплой, выбираем компонент & выбираем ip адрес
>
> И нажимаем Deploy
>
> В виджет Мониторинг, будет отображатся активная машина и апликейшен# dscontrol
# test1
