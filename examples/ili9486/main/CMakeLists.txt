idf_component_register(SRCS main.c
						INCLUDE_DIRS "." 
						"../../../ui_component/src_generated"
						

						PRIV_INCLUDE_DIRS 
						"../../../ui_component/runtime"
						"../../../ui_component/assets/src_generated"


						#PRIV_REQUIRES "esp-lcd-device"
						)