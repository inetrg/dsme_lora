LORA_OMNETPP=../src/dsme_lora
ROOT_PATH=../
INET_PATH=$ROOT_PATH/inet
FLORA_PATH=$ROOT_PATH/flora
NUM_CORES=4
INET_LIB=$INET_PATH/src/INET

NED_INCLUDES=../src:.:$INET_PATH/src:$INET_PATH/examples:$INET_PATH/tutorials:$INET_PATH/showcases:$FLORA_PATH/src
OMNETPP_LIBS="-l  $INET_LIB"

for f in $(find s_*.ini)
do
    LORA_OMNETPP_ARGS="-m -n $NED_INCLUDES --image-path=$INET_PATH/images $OMNETPP_LIBS $OMNETPP_EXTRA_ARGS -f $f -f common.ini"
    opp_runall -j$NUM_CORES -b1 $LORA_OMNETPP $LORA_OMNETPP_ARGS
done
