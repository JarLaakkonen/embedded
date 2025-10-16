*** Settings ***
Library   String
Library   SerialLibrary

*** Variables ***
${com}   	COM5
${baud} 	115200
${board}	nRF
${test1}      000120X
${test2}      048265X
${test3}      0-1212X
${test4}      	 0120X
${test5}      0A0120X
${test6}      NULLX
${test7}      000000X
${LEN_ERROR}	-1X
${time_pass}    80X
${TIME_VALUE_OVERFLOW_ERROR} 	-22X
${TIME_VALUE_NEGATIVE_ERROR} 	-33X	
${TIME_VALUE_SIZE_ERROR}	 	-44X
${TIME_VALUE_CHAR_ERROR}	 	-55X
${TIME_SECONDS_ZERO_ERROR}	 	-77X				
${light_test1}					yrgX
${light_test2}					G1GYX
${light_test3}					G-RYX
${light_test4}					GRBX
${light_test5}					GRGX
${LIGHT_LOWER_CASE_ERROR}	  	-2X
${LIGHT_NUMERIC_ERROR}		  	-3X
${LIGHT_NOT_CHAR_ERROR}		  	-4X
${LIGHT_INVALID_VALUE_ERROR}  	-5X
${LIGHT_VALUES_OK}  		  	 0X

*** Test Cases ***
Connect Serial
	Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  encoding=ascii
	Port Should Be Open  ${com}
	Reset Input Buffer
	Reset Output Buffer

Serial HHMMSS time_pass_test
	Write Data   ${test1}   encoding=ascii 
	Log To Console   Send sequence ${test1}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${time_pass}
	Log To Console   Tested ${read} is same as ${time_pass}
Serial HHMMSS High_TIME_Error
	Write Data   ${test2}   encoding=ascii 
	Log To Console   Send sequence ${test2}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${TIME_VALUE_OVERFLOW_ERROR}
	Log To Console   Tested ${read} is same as ${TIME_VALUE_OVERFLOW_ERROR}
Serial HHMMSS TIME_VALUE_NEGATIVE_ERROR
	Write Data   ${test3}   encoding=ascii 
	Log To Console   Send sequence ${test3}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${TIME_VALUE_NEGATIVE_ERROR}
	Log To Console   Tested ${read} is same as ${TIME_VALUE_NEGATIVE_ERROR}
Serial HHMMSS TIME_VALUE_SIZE_ERROR
	Write Data   ${test4}   encoding=ascii 
	Log To Console   Send sequence ${test4}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${TIME_VALUE_SIZE_ERROR}
	Log To Console   Tested ${read} is same as ${TIME_VALUE_SIZE_ERROR}
Serial HHMMSS TIME_VALUE_CHAR_ERROR
	Write Data   ${test5}   encoding=ascii 
	Log To Console   Send sequence ${test5}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${TIME_VALUE_CHAR_ERROR}
	Log To Console   Tested ${read} is same as ${TIME_VALUE_CHAR_ERROR}
Serial HHMMSS TIME_SECONDS_ZERO_ERROR
	Write Data   ${test7}   encoding=ascii 
	Log To Console   Send sequence ${test7}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${TIME_SECONDS_ZERO_ERROR}
	Log To Console   Tested ${read} is same as ${TIME_SECONDS_ZERO_ERROR}
Serial LIGHT LIGHT_LOWER_CASE_ERROR
	Write Data   ${light_test1}   encoding=ascii 
	Log To Console   Send sequence ${light_test1}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${LIGHT_LOWER_CASE_ERROR}
	Log To Console   Tested ${read} is same as ${LIGHT_LOWER_CASE_ERROR}
Serial LIGHT LIGHT_NUMERIC_ERROR
	Write Data   ${light_test2}   encoding=ascii 
	Log To Console   Send sequence ${light_test2}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${LIGHT_NUMERIC_ERROR}
	Log To Console   Tested ${read} is same as ${LIGHT_NUMERIC_ERROR}
Serial LIGHT LIGHT_NOT_CHAR_ERROR
	Write Data   ${light_test3}   encoding=ascii 
	Log To Console   Send sequence ${light_test3}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${LIGHT_NOT_CHAR_ERROR}
	Log To Console   Tested ${read} is same as ${LIGHT_NOT_CHAR_ERROR}
Serial LIGHT LIGHT_INVALID_VALUE_ERROR
	Write Data   ${light_test4}   encoding=ascii 
	Log To Console   Send sequence ${light_test4}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${LIGHT_INVALID_VALUE_ERROR}
	Log To Console   Tested ${read} is same as ${LIGHT_INVALID_VALUE_ERROR}
Serial LIGHT LIGHT_VALUES_OK
	Write Data   ${light_test5}   encoding=ascii 
	Log To Console   Send sequence ${light_test5}
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console   Received ${read}
	Should Be Equal As Strings   ${read}    ${LIGHT_VALUES_OK}
	Log To Console   Tested ${read} is same as ${LIGHT_VALUES_OK}














Disconnect Serial
	Log To Console  Disconnecting ${board}
	[TearDown]  Delete Port  ${com}


	
	
	

