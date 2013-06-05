CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(LIB_DIR)/libCCeil.a $(BIN_DIR)/CCeil

SRC = \
CCeil.cpp \
CCeilLArgs.cpp \
CCeilLBlock.cpp \
CCeilLCntrl.cpp \
CCeilLCommand.cpp \
CCeilLConvert.cpp \
CCeilLDebug.cpp \
CCeilLError.cpp \
CCeilLFunc.cpp \
CCeilLLabel.cpp \
CCeilLModule.cpp \
CCeilLProc.cpp \
CCeilLSignal.cpp \
CCeilLStdCmd.cpp \
CCeilPArray.cpp \
CCeilPDict.cpp \
CCeilPEval.cpp \
CCeilPFunc.cpp \
CCeilPIdent.cpp \
CCeilPIFunc.cpp \
CCeilPInit.cpp \
CCeilPInteger.cpp \
CCeilPList.cpp \
CCeilPObj.cpp \
CCeilPOp.cpp \
CCeilPParse.cpp \
CCeilPProcess.cpp \
CCeilPReal.cpp \
CCeilPScope.cpp \
CCeilPStack.cpp \
CCeilPString.cpp \
CCeilPStruct.cpp \
CCeilPType.cpp \
CCeilPUserfn.cpp \
CCeilPValue.cpp \
CCeilPVar.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS = -lCCeil -lCArgs -lCOS -lCStrUtil -lCGlob

CPPFLAGS = \
-I$(INC_DIR) \
-I../../CArgs/include \
-I../../COS/include \
-I../../CStrUtil/include \
-I../../CGlob/include \
-I../../CMath/include \
-I../../CUtil/include \
-I.

LFLAGS = -L$(LIB_DIR)

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCCeil.a
	$(RM) -f $(BIN_DIR)/CCeil

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCCeil.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCCeil.a $(OBJS)

$(BIN_DIR)/CCeil: $(OBJ_DIR)/CCeil.o $(LIB_DIR)/libCCeil.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CCeil $(OBJ_DIR)/CCeil.o $(LFLAGS) $(LIBS)
