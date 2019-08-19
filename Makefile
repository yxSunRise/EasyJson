INCLUDE=-I./ \
-I./jsoncpp/include

VPATH+=./ \
./jsoncpp/lib

FLAGS=-std=c++11 -Wno-narrowing -w

obj = main.o json_reader.o json_value.o json_writer.o

#$@:$^
#$< 表示$^中第一个
EasyJson:${obj}
#g++ 编译生成bin文件，.o 文件一定要放在第一个位置上,选项放后面 , TNND
	g++ $^ ${FLAGS} ${LIB_PATH} ${LINK} ${INCLUDE} -o $@

%.o:%.cpp
	g++ -c ${FLAGS} ${INCLUDE} ${LIB_PATH} ${LINK} $< -o $@
