
px4_add_module(
	MODULE mypublish
	MAIN mypublish
	STACK_MAIN 2000
	SRCS
		mypub_main.cpp
	DEPENDS
)
