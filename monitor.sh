echo ${1}
rm -f ${1}
while true; do (echo "%CPU %MEM ARGS $(date)" && ps -e -T -o pcpu,pmem,args,psr,rss --sort=pcpu | grep ./cli && smem -P ./cli) >> ${1}; sleep 0.2; done
