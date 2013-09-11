TARGETS    = simpleDes
DEPS       = boxes/libboxes.a
SNETCFLAGS = -static -g -Lboxes -lboxes

include $(SNET_MISC)/snet.mkf

$(DEPS):
	make -C boxes/
