set -xe

# Usage: ./fg_perf <seconds_to_run> <output_svg_name>

# Start prof
# DRI_PRIME=1 __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia __GL_SYNC_TO_VBLANK=0 ./gui &
./gui &
PID=$(pidof gui)

# record with perf for $1 seconds
perf record -F 99 -p  $PID -a -g -- sleep $1
perf script > perf_script.perf

mkdir -p perf_img

# folding and fg gen
FG_PATH="/home/hekapoo/Documents/tools/Flamegraph"
"$FG_PATH/stackcollapse-perf.pl" perf_script.perf > folded.out
"$FG_PATH/flamegraph.pl" folded.out > perf_img/$2

# cleanup
rm -rf perf_script.perf folded.out perf.data perf.data.old
kill $PID