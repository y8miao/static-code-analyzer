TARGET=test3

all: $(TARGET).bc outputs

include ../pipair.mak

$(TARGET).bc: httpd-2.2.21.bc.orig
	cp $< $@

clean:
	rm -rf $(TARGET).bc
	rm -rf $(TARGET)_*_*.out


