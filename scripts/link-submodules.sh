#!/bin/sh

git submodule add git@github.com:bdmihai/stm32-doc doc/stm32
git submodule add git@github.com:bdmihai/stm32-qbs qbs/modules/stm32
git submodule add git@github.com:bdmihai/stm32-startup source/startup
git submodule add git@github.com:bdmihai/stm32-linker source/linker
git submodule add git@github.com:bdmihai/stm32-hal source/hal
git submodule add git@github.com:bdmihai/stm32-cmsis source/cmsis
git submodule add git@github.com:bdmihai/stm32-freertos source/freertos
