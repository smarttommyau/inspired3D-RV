default: all


all:
	$(MAKE) -C pong
	$(MAKE) -C test_buttons_detection
	$(MAKE) -C test_inspire3d_screen
	$(MAKE) -C test_inspire3d_screen_control
	$(MAKE) -C testing

clean:
	$(MAKE) -C pong clean
	$(MAKE) -C test_buttons_detection clean
	$(MAKE) -C test_inspire3d_screen clean
	$(MAKE) -C test_inspire3d_screen_control clean
	$(MAKE) -C testing clean