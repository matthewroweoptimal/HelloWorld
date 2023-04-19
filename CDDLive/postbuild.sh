#/bin/sh
/bin/echo === post build start ===

/bin/echo Creating CDDLive.bin
${cross_prefix}${cross_objcopy}${cross_suffix} -O binary "${ProjName}.elf" "${ProjName}.bin"


/bin/echo Delay 10s
sleep 10

/bin/echo Creating CDDLive.bin.fw
${ProjDirPath}/TOOLS/OlyPack/Release/OlyPack.exe -L ${ProjName}.log -I ${ProjName}..bin -O ${ProjName}.Fw.bin -V 1.0.0 -S 0x0 -T 0x0 -P 0x28000

/bin/echo === post build end ===