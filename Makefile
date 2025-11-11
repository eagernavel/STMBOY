CMAKE_PRESET = stm32f411re

all:
	cmake --build --preset $(CMAKE_PRESET) --target all

clean: 
	cmake --build --preset $(CMAKE_PRESET) --target clean

configure:
	cmake --preset $(CMAKE_PRESET)

flash:
	sudo /opt/xpack-openocd-0.12.0-7/bin/openocd -f "interface/stlink.cfg" -f "target/stm32f4x.cfg" -c "program build/$(CMAKE_PRESET)/source/firmware.bin verify reset exit 0x08000000"
