def generate_setter(x)
    print "/* Name: Set#{x}
 * Description: Sets the #{x} and verify if the input is acceptable
 * Inputs:	 Raw string containing the value [char*]
 * Outputs:  Whether the form is still OK or not [BYTE]
 */\r\n"
end
def generate_getters(x,y)
  print "/* Name: Get#{x}
 * Description: Gets the last valid value from
 * Inputs:	 void
 * Outputs:  The value of the field #{y}
 */"
end
functions_setter = [	'Direction' ,'MaxRPM','MinRPM','Rotations','Steps']
functions_setter.each { |k| generate_setter(k)}
functions = ['Direction' ,'MaxRPM','MinRPM','Rotations','Steps','Mode']
functions_return = ['BYTE' ,'int','int','int','int','BYTE']
0.upto(5)  {|i| generate_getters(functions[i],functions_return[i])}
