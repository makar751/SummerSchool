cmd_/home/user/newmod/module.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /home/user/newmod/module.ko /home/user/newmod/module.o /home/user/newmod/module.mod.o
