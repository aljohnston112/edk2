GDB_COMMANDS := gdb_commands
MAKE_OUTPUT := make_output.txt

all: build compiledb run \
	 Application/BootLib \
     FourthFingerPkg.dec \
     FourthFingerPkg.dsc

all_debug: build compiledb run_debug \
	 Application/BootLib \
     FourthFingerPkg.dec \
     FourthFingerPkg.dsc

build:
	pwd && make VERBOSE=y _build &> $(MAKE_OUTPUT)

_build:
	cd .. && pwd && . ./edksetup.sh && build

compiledb:
	pwd && compiledb --parse $(MAKE_OUTPUT) && rm $(MAKE_OUTPUT)

run:
	cd .. && pwd && \
	cd ./Build/FourthFingerPkg/DEBUG_GCC5/X64/ && \
	qemu-system-x86_64 -enable-kvm -cpu host -drive if=pflash,format=raw,readonly=on,file=../../../OvmfX64/DEBUG_GCC5/FV/OVMF_CODE.fd -drive if=pflash,format=raw,file=../../../OvmfX64/DEBUG_GCC5/FV/OVMF_VARS.fd -drive file=fat:rw:.,format=raw


run_debug:
	cd .. && pwd && \
	cp ./Build/FourthFingerPkg/DEBUG_GCC5/X64/FourthFingerPkg.efi ./Build/EmulatorX64/DEBUG_GCC55/X64/FourthFingerPkg.efi && \
	echo "add-symbol-file ../../../FourthFingerPkg/DEBUG_GCC5/X64/FourthFingerPkg.debug" > $(GDB_COMMANDS) && \
	EmulatorPkg/build.sh run < $(GDB_COMMANDS) && \
	rm $(GDB_COMMANDS)

clean:
	@if [ -d "../Build/FourthFingerPkg" ]; then \
		rm -rf "../Build/FourthFingerPkg"; \
	fi