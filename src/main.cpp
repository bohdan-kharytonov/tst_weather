#include <iostream>
#include <memory>
#include <QWidget>
#include <QObject>
#include <QApplication>
#include <QPushButton>
#include <thread> 

#include "include/bmp280data.h"
#include "include/RawData.h"
#include "include/data_manager.h"
#include "include/form.h"
#include "include/settings_pop_up.h"
#include "include/mki109v1.h"
#include "include/aws.h"

#include <aws/crt/Api.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ApiHandle apiHandle;
    aws a;
    mki109v1 mki;
    Settings settings;
    

    std::unique_ptr<DataManager> dtrt = std::make_unique<DataManager>(settings,mki);
    a.connect();
    a.subscribe("sdk/tst_weather/sub",[&](std::string data){dtrt->command_selector(data,a);});
    Form main_window(settings);
    settings_pop_up set;

    BMP280Data *bmp_data = new BMP280Data;
    RawData *raw_data = new RawData;

    if(bmp_data->init() ==  true){
        dtrt->class_manager(raw_data);
    }
    else{
        dtrt->class_manager(bmp_data);
    }

    QObject::connect(&main_window, &Form::setting_request,&set, [&set](){set.show();} );

    QObject::connect(&set, &settings_pop_up::temp_units_currentIndexChanged, &main_window, &Form::handleTempUnitChange );
    QObject::connect(&set, &settings_pop_up::pressure_units_currentIndexChanged, &main_window, &Form::handlePressUnitChange );

    QObject::connect(dtrt.get(), &DataManager::tempChange, &main_window, &Form::takeTemp);
    QObject::connect(dtrt.get(), &DataManager::pressChange, &main_window, &Form::takePressure);

    QObject::connect(&mki, &mki109v1::magneticFieldChange, &main_window, &Form::getMagneticField);
    main_window.show();
    dtrt->data_manager();

    return app.exec();

}
