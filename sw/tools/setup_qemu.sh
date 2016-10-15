#!/bin/sh

INSTALLDIR=$1
QEMU_SYSTEM_I386=qemu/i386-softmmu/qemu-system-i386

[ $# -eq 1 ] && cd $INSTALLDIR
[ -f $QEMU_SYSTEM_I386 ] && echo "Nothing to be done" && exit 0

echo '[+] Download QEMU'
git clone --depth 1 --branch stable-2.4 git://git.qemu.org/qemu.git
cd qemu

echo '[+] Apply patch: read/write bios memory regions'
git apply <<'EOF'
diff --git a/hw/i386/pc_sysfw.c b/hw/i386/pc_sysfw.c
index 662d997..d473a19 100644
--- a/hw/i386/pc_sysfw.c
+++ b/hw/i386/pc_sysfw.c
@@ -70,7 +70,9 @@ static void pc_isa_bios_init(MemoryRegion *rom_memory,
            ((uint8_t*)flash_ptr) + (flash_size - isa_bios_size),
            isa_bios_size);
 
+#if 0
     memory_region_set_readonly(isa_bios, true);
+#endif
 }
 
 #define FLASH_MAP_UNIT_MAX 2
@@ -195,9 +197,13 @@ static void old_pc_system_rom_init(MemoryRegion *rom_memory, bool isapc_ram_fw)
     bios = g_malloc(sizeof(*bios));
     memory_region_init_ram(bios, NULL, "pc.bios", bios_size, &error_abort);
     vmstate_register_ram_global(bios);
+
+#if 0
     if (!isapc_ram_fw) {
         memory_region_set_readonly(bios, true);
     }
+#endif
+
     ret = rom_add_file_fixed(bios_name, (uint32_t)(-bios_size), -1);
     if (ret != 0) {
     bios_error:
@@ -218,9 +224,12 @@ static void old_pc_system_rom_init(MemoryRegion *rom_memory, bool isapc_ram_fw)
                                         0x100000 - isa_bios_size,
                                         isa_bios,
                                         1);
+
+#if 0
     if (!isapc_ram_fw) {
         memory_region_set_readonly(isa_bios, true);
     }
+#endif
 
     /* map all the bios at the top of memory */
     memory_region_add_subregion(rom_memory,
EOF

echo '[+] Build QEMU'
./configure --target-list=i386-softmmu --python=`command -v python2`
make -j2

echo '[+] Done'
