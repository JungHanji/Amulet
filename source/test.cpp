#include <Amulet/Logging/logger.hpp>

using namespace amulet::logging;

int main (){
    Logger logger(
        BOTH, // Тип логгирования CONSOLE_LOGGING для консоли, FILE_LOGGING для файлового и BOTH для обоих 
        "example.log", // Путь до файла с логом
        false, // Стереть предыдущие записис в файле?
        true, // Логгировать переход уровней?
        true, // Добавить дату к названию файла и вывести эту дату?
        true  // Активен ли сейчас логгер?
    );

    // logger.activate(); Активировать логгер

    logger.newlayer("First layer");
    logger.log(DEBUG, "Debug info");
    logger.log(INFO, "Info message");
    logger.log(WARNING, "Warning message");
    logger.log(ERROR, "Error message");
    logger.log(FATAL, "Fatal error message");
    logger.newlayer("Second layer");
    logger.log(INITED, "Something inited");
    logger.log(CALLING, "Function call");
    logger.poplayer();

    auto bak = logger.getlayers(); // Создадим бэкап текущих слоёв
    logger.setlayers({"class", "method"});
    logger.log(INFO, "Info message in class.method");
    logger.setlayers(bak); // Восстановим текущие слои

    logger.setlayer("New layer"); // Переклучим слой на данный
    logger.log(DEBUG, "Debug message in new layer");

    logger.deactivate(); // Деактивировать логгер 
}