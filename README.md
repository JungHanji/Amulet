# Проект Amulet

## Зачем нужен этот проект

Этот проект нужен для развертывания системы сервер-клиент на системах под управлением систем на основе UNIX

## Что этот проект позволяет сделать

- Клиента (он синхронный, но ничто не останавливает вас от того, чтобы запускать его в отдельном потоке)
- Разные сервера, а именно:
  - Сервер синхронный, т.е. блокирующий. Он обрабатывает запросы от клиентов в порядке очереди, поэтому в реальных проектах неэффективен
  - Сервер асинхронный. Он обрабатывает запросы от клиентов параллельно, но если один из клиентов долго не отвечает, сервер не сможет дальше продолжить работу. Т.е. скорость обработки запросов зависит от того, на сколько долго будет отвечать самый медленный клиент
  - Сервер «отсоединенный». Он обрабатывает запросы от клиентов в отдельных потоках, поэтому даже самый медленный клиент не сможет негативно повлиять на обработку других клиентов

## Компиляция проекта

Для компиляции нужен компилятор `C++` кода, например `G++` или `Clang++`, а также `Python` интерпретатор

```shell
python ./tools/compile.py ./source/blockingClient # Для компиляции с помощью G++
python ./tools/compile.py ./source/asyncServer clang++ # Для компиляции с помощью Clang++
```

## Другая информация

Клиент и сервер могут вести подробные логи, как в консоль, так и в файл
