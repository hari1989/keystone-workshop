## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/vlfft_evmc6678l_pe66.oe66 \

linker.cmd: package/cfg/vlfft_evmc6678l_pe66.xdl
	$(SED) 's"^\"\(package/cfg/vlfft_evmc6678l_pe66cfg.cmd\)\"$""\"C:/dev/keystone-workshop/examples/vlfft/evmc6678l/Debug/configPkg/\1\""' package/cfg/vlfft_evmc6678l_pe66.xdl > $@
