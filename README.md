# Raspberry-Pi-5-Argon-IR-control
Argon40 ir remote control configuration for Raspberry pi 5 with case Argon ONE v3 in Lineage OS 

Pasos para configurar el control remoto Argon40 en Argon ONE v3 para Raspberry Pi 5 con:
Lineage lineage-23.2-20260128-UNOFFICIAL-KonstaKANG-rpi5-atv-ota.zip "https://konstakang.com/devices/rpi5/LineageOS23-ATV/"
MindTheGapps/16.0.0-arm64-ATV "https://github.com/MindTheGapps/16.0.0-arm64-ATV"
guia para instalar en https://pimylifeup.com/raspberry-pi-android-tv-lineageos/

Descargar Android Debug Tools adb "https://developer.android.com/tools/adb?hl=es-419"

Revisar que el pin del ir este en el correspondiente (en este caso el 23) en:
/boot/config.txt

apoyándonos en los comandos:
adb shell dumpsys input
adb shell cat /proc/device-tree/ir-receiver@12/gpios
adb shell cat /proc/bus/input/devices

crear un archivo argon_ir_mapper.c

compilar con Android NDK:
android-ndk-r27d-windows\android-ndk-r27d\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android21-clang -O2 -o argon_ir_mapper argon_ir_mapper.c

Habilitar el modo de desarrollo en la Raspberry Argon40 ONE V3 y conectarse con:

adb.exe connect [ip aquí sin los corchetes]
adb root
adb remount
adb push argon_ir_mapper /vendor/bin/
adb shell chmod 755 /vendor/bin/argon_ir_mapper

crear el servicio argon_ir.rc 

copiar a la "TV":

adb push argon_ir.rc /vendor/etc/init/
adb shell chmod 644 /vendor/etc/init/argon_ir.rc
adb reboot

si funciona somos felices XD
