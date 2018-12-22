px4_add_module(
	MODULE examples__mysubscribe
	MAIN mysubscribe
	STACK_MAIN 2000
	SRCS
		mysub_main.cpp
	DEPENDS
	)
