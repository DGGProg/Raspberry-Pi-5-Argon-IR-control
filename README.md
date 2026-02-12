# Raspberry-Pi-5-Argon-IR-control
Argon40 ir remote control configuration for Raspberry pi 5 with case Argon ONE v3 in Lineage OS 

Argon40 IR Remote Setup
Argon ONE v3 – Raspberry Pi 5 – Android TV (LineageOS)

🇺🇸 English
📖 Description

This guide explains how to enable and configure the Argon40 IR remote on an Argon ONE v3 case running LineageOS Android TV on a Raspberry Pi 5.

✅ Tested With

Raspberry Pi 5

Argon ONE v3

LineageOS 23 Android TV

MindTheGapps Android TV

Argon40 Remote

📦 Required Downloads
LineageOS Android TV

https://konstakang.com/devices/rpi5/LineageOS23-ATV/

Tested build:

lineage-23.2-20260128-UNOFFICIAL-KonstaKANG-rpi5-atv-ota.zip

MindTheGapps Android TV

https://github.com/MindTheGapps/16.0.0-arm64-ATV

Installation Guide

https://pimylifeup.com/raspberry-pi-android-tv-lineageos/

Android Debug Bridge (ADB)

https://developer.android.com/tools/adb

⚙️ Step 1 – Verify IR GPIO Pin

The Argon40 IR receiver uses GPIO 23.

Edit:

/boot/config.txt


Ensure this line exists:

dtoverlay=gpio-ir,gpio_pin=23

🔍 Verify IR Detection

Run:

adb shell dumpsys input
adb shell cat /proc/device-tree/ir-receiver@12/gpios
adb shell cat /proc/bus/input/devices


You should see:

gpio_ir_recv

⚙️ Step 2 – Create IR Mapper

Create:

argon_ir_mapper.c


This program:

Reads IR scan codes

Converts them to Android key events

Uses uinput to create a virtual keyboard

⚙️ Step 3 – Compile Mapper Using Android NDK
aarch64-linux-android21-clang -O2 -o argon_ir_mapper argon_ir_mapper.c

⚙️ Step 4 – Enable Developer Mode

Android TV:

Settings → Device Preferences → About → Tap Build Number multiple times


Enable:

USB Debugging

Root ADB

⚙️ Step 5 – Connect ADB
adb connect <RASPBERRY_IP>
adb root
adb remount

⚙️ Step 6 – Install Binary
adb push argon_ir_mapper /vendor/bin/
adb shell chmod 755 /vendor/bin/argon_ir_mapper

⚙️ Step 7 – Create Boot Service

Create:

argon_ir.rc


Example:

service argon_ir /vendor/bin/argon_ir_mapper
    class main
    user root
    group root input uhid
    oneshot

⚙️ Step 8 – Enable NEC Protocol at Boot
write /sys/class/rc/rc0/protocols "lirc nec"

⚙️ Step 9 – Install Service
adb push argon_ir.rc /vendor/etc/init/
adb shell chmod 644 /vendor/etc/init/argon_ir.rc
adb reboot

🧪 Testing
adb shell getevent

🛠 Troubleshooting
Remote not detected
cat /sys/class/rc/rc0/protocols

Should include:

nec

Service not starting
adb logcat

Permission issues
chmod 755 /vendor/bin/argon_ir_mapper

🇪🇸 Español
📖 Descripción

Esta guía explica cómo habilitar y configurar el control remoto IR Argon40 en un gabinete Argon ONE v3 ejecutando Android TV LineageOS en una Raspberry Pi 5.

✅ Probado Con

Raspberry Pi 5

Argon ONE v3

LineageOS 23 Android TV

MindTheGapps Android TV

Control remoto Argon40

📦 Descargas Necesarias
LineageOS Android TV

https://konstakang.com/devices/rpi5/LineageOS23-ATV/

Ejemplo probado:

lineage-23.2-20260128-UNOFFICIAL-KonstaKANG-rpi5-atv-ota.zip

MindTheGapps Android TV

https://github.com/MindTheGapps/16.0.0-arm64-ATV

Guía de instalación

https://pimylifeup.com/raspberry-pi-android-tv-lineageos/

Android Debug Bridge (ADB)

https://developer.android.com/tools/adb

⚙️ Paso 1 – Verificar Pin GPIO del IR

El receptor IR Argon40 usa el GPIO 23.

Editar:

/boot/config.txt


Verificar que exista:

dtoverlay=gpio-ir,gpio_pin=23

🔍 Verificar detección del IR

Ejecutar:

adb shell dumpsys input
adb shell cat /proc/device-tree/ir-receiver@12/gpios
adb shell cat /proc/bus/input/devices


Debe aparecer un dispositivo similar a:

gpio_ir_recv

⚙️ Paso 2 – Crear Mapper IR

Crear archivo:

argon_ir_mapper.c


Este programa:

Lee códigos IR

Los convierte a eventos de teclado Android

Usa uinput para generar un teclado virtual

⚙️ Paso 3 – Compilar Mapper con Android NDK
aarch64-linux-android21-clang -O2 -o argon_ir_mapper argon_ir_mapper.c


Ejemplo en Windows:

android-ndk-r27d-windows\android-ndk-r27d\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android21-clang -O2 -o argon_ir_mapper argon_ir_mapper.c

⚙️ Paso 4 – Activar Modo Desarrollador

En Android TV:

Configuración → Preferencias del dispositivo → Información → Presionar varias veces "Build Number"


Activar:

Depuración USB

Root ADB

⚙️ Paso 5 – Conectar ADB
adb connect <IP_RASPBERRY>
adb root
adb remount

⚙️ Paso 6 – Instalar Binario
adb push argon_ir_mapper /vendor/bin/
adb shell chmod 755 /vendor/bin/argon_ir_mapper

⚙️ Paso 7 – Crear Servicio de Inicio

Crear archivo:

argon_ir.rc


Ejemplo:

service argon_ir /vendor/bin/argon_ir_mapper
    class main
    user root
    group root input uhid
    oneshot

⚙️ Paso 8 – Habilitar Protocolo NEC al Arranque

Agregar:

write /sys/class/rc/rc0/protocols "lirc nec"

⚙️ Paso 9 – Instalar Servicio
adb push argon_ir.rc /vendor/etc/init/
adb shell chmod 644 /vendor/etc/init/argon_ir.rc
adb reboot

🧪 Pruebas
adb shell getevent

Presionar botones del control.

🛠 Solución de Problemas
IR no detectado
cat /sys/class/rc/rc0/protocols


Debe incluir:

nec

Servicio no inicia
adb logcat

Problemas de permisos
chmod 755 /vendor/bin/argon_ir_mapper

