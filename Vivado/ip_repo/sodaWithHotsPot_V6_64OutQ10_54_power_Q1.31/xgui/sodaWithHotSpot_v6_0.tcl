# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "FLOAT_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "INT_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "SIZE" -parent ${Page_0}


}

proc update_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to update DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to validate DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.FLOAT_WIDTH { PARAM_VALUE.FLOAT_WIDTH } {
	# Procedure called to update FLOAT_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FLOAT_WIDTH { PARAM_VALUE.FLOAT_WIDTH } {
	# Procedure called to validate FLOAT_WIDTH
	return true
}

proc update_PARAM_VALUE.INT_WIDTH { PARAM_VALUE.INT_WIDTH } {
	# Procedure called to update INT_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.INT_WIDTH { PARAM_VALUE.INT_WIDTH } {
	# Procedure called to validate INT_WIDTH
	return true
}

proc update_PARAM_VALUE.SIZE { PARAM_VALUE.SIZE } {
	# Procedure called to update SIZE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.SIZE { PARAM_VALUE.SIZE } {
	# Procedure called to validate SIZE
	return true
}


proc update_MODELPARAM_VALUE.DATA_WIDTH { MODELPARAM_VALUE.DATA_WIDTH PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.DATA_WIDTH}] ${MODELPARAM_VALUE.DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.INT_WIDTH { MODELPARAM_VALUE.INT_WIDTH PARAM_VALUE.INT_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.INT_WIDTH}] ${MODELPARAM_VALUE.INT_WIDTH}
}

proc update_MODELPARAM_VALUE.FLOAT_WIDTH { MODELPARAM_VALUE.FLOAT_WIDTH PARAM_VALUE.FLOAT_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FLOAT_WIDTH}] ${MODELPARAM_VALUE.FLOAT_WIDTH}
}

proc update_MODELPARAM_VALUE.SIZE { MODELPARAM_VALUE.SIZE PARAM_VALUE.SIZE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.SIZE}] ${MODELPARAM_VALUE.SIZE}
}

