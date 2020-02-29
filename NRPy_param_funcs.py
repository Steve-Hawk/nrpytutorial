# As documented in the NRPy+ tutorial module
#   Tutorial-Coutput__Parameter_Interface.ipynb
#   this core NRPy+ module is used for
#   initializing, storing, and recalling
#   parameters.

# Author: Zachariah B. Etienne
#         zachetie **at** gmail **dot* com

import sympy as sp                   # Import SymPy
import os, sys                       # Standard Python: OS-independent system functions
from collections import namedtuple   # Standard Python: Enable namedtuple data type
### 具名元组的使用更加偏向于c++中类的初始化
glb_params_list = []  # = where we store NRPy+ parameters and default values of parameters. A list of named tuples
glb_paramsvals_list = []  # = where we store NRPy+ parameter values.
glb_param  = namedtuple('glb_param', 'type module parname defaultval')
### 上面其实可以直观地理解为在某个模块中定义了参数（变量），给出他的类型以及默认值（传统的C++的代码风格写法）
glb_Cparams_list = []  # = where we store C runtime parameters and default values of parameters. A list of named tuples
glb_Cparam = namedtuple('glb_Cparam','type module parname defaultval')

veryverbose = False
### 检查现有glb_params_list是否有符合条件的参数，没有的话添加这个参数项并且增添默认值
def initialize_param(input):
    if get_params_idx(input) == -1:
        glb_params_list.append(input)
        glb_paramsvals_list.append(input.defaultval)
    else:
        if veryverbose == True:
            print("initialize_param() minor warning: Did nothing; already initialized parameter "+input.module+"::"+input.parname)
### 检查现有glb_Cparams_list是否有符合条件的参数，没有的话添加这个参数项
def initialize_Cparam(input):
    if get_params_idx(input,Cparam=True) == -1:
        glb_Cparams_list.append(input)
    else:
        if veryverbose == True:
            print("initialize_Cparam() minor warning: Did nothing; already initialized parameter "+input.module+"::"+input.parname)

# Given the named tuple `input` and list of named tuples `params`,
#    defined according to namedtuple('param', 'type module name defaultval'),
#    where in the case of `input`, defaultval need not be set,
#    return the list index of `params` that matches `input`.
#### 从已有的参数列表中遍历复合的情况，输入直接是一个namedtuple,并返回在参数列表中的下标，没有找到的情况下返回-1
# On error returns -1
# parameter input only support single namedtuple
def get_params_idx(input,Cparam=False):
    # inspired by: https://stackoverflow.com/questions/2917372/how-to-search-a-list-of-tuples-in-python:
    if Cparam==False:
        list = [i for i, v in enumerate(glb_params_list)
                if (input.type=="ignoretype" or input.type==v[0]) and input.module == v[1] and input.parname == v[2]]
    else:
        list = [i for i, v in enumerate(glb_Cparams_list) if input.parname == v[2]]
    if list == []:
        return -1 # No match found => error out!
    else:
        if len(list) > 1:
            print("Error: Found multiple parameters matching "+str(input))
            sys.exit(1)
        return list.pop() # pop() returns the index
### 获取输入参数的名字的同时检索参数列表中是否存在，返回参数列表中的默认值
def get_params_value(input):
    idx = get_params_idx(input)
    if idx < 0:
        print("Error: could not find a parameter matching:",input)
        print("Full list of modules:\n",glb_params_list)
        sys.exit(1)
    else:
        return glb_paramsvals_list[idx]

### 使用变量名以及模块名的方式找到在参数列表中的对应位置，找不到的情况下直接报错，支持类似与modname::varname风格的输入
def idx_from_str(varname,modname=""):
    if "::" in varname:
        splitstring = re.split('::', varname)
        modname=splitstring[0]
        varname=splitstring[1]
        
    # inspired by: https://stackoverflow.com/questions/2917372/how-to-search-a-list-of-tuples-in-python:    
    if modname == "":
        list = [i for i, v in enumerate(glb_params_list) if v[2] == varname]
    else:
        list = [i for i, v in enumerate(glb_params_list) if (v[1] == modname and v[2] == varname)]
    if list == []:
        print("Error: Could not find a parameter matching \""+varname+"\" in ",glb_params_list)
        sys.exit(1)
    if len(list) > 1:
        print("Error: Found more than one parameter named \""+varname+"\". Use get_params_value() instead.")
        sys.exit(1)
    return list.pop()
### 通过给出modname::varname风格的输入返回对应到参数列表中的默认值
def parval_from_str(string):
    return glb_paramsvals_list[idx_from_str(string)]
### 用来修改输入为modname::varname情况下的默认值，有什么其他作用？
def set_parval_from_str(string,value):
    glb_paramsvals_list[idx_from_str(string)] = value

# Summary: This function parses a string like this
# module::variablename=value
# into its component parts: module,variablename,value
# then hunts for module,variablename in the
# params list. When it finds the matching index in the
# list "idx", it sets paramsvals[idx] = value.
# Usage comment: This function parses both parameter file
#   inputs as well as parameter inputs from the command
#   line. You should set filename = "" if reading from the
#   command line. This ensures the error messages are
#   appropriate for the context.
import re

def set_paramsvals_value(line,filename="", FindMainModuleMode=False):
    ### 虽然没有体现filename的使用，但是实际在读入文件内容的时候需要指明文件名同时
    ### par.set_paramsvals_value(line, sys.argv[1], FindMainModuleMode=True) nrpy.py的调用时候使用
    MainModuleFound = True
    if FindMainModuleMode == True:
        MainModuleFound = False
    ### 移除字符串头尾处的空格
    stripped_line_of_text = line.strip()
    ### 文件中的内容每行不能以一个#开头，直接报错
    if not stripped_line_of_text.startswith("#"):
        # Valid lines take the form:
        #    module::variable = value # Comment here
        # Thus, using the delimiters "::", "=", and "#",
        # with the regex split command, the first three
        # items in single_param_def will be [module, variablename, value]
        ### 分解得到对应列表值，满足上述形式[module, variablename, value]，其中可能仍有可能含有空格
        single_param_def = re.split('::|=|#', stripped_line_of_text)

        # First verify that single_param_def has at least 3 parts:
        ### 如果不是从usr-friendly角度，多此一举
        if len(single_param_def) < 3:
            if filename != "":
                print("Error: the line " + line + " in parameter file " + filename + " is not in the form")
                print("\"module::variable = value\"")
            else:
                print("Error: the command-line argument " + stripped_line_of_text + " is not in the form")
                print("\"module::variable=value\"   <-- NOTICE NO SPACES ALLOWED!")
            sys.exit(1)

        # Next remove all leading/trailing whitespace from single_param_def
        ### 再移除通过分割字符列表中可能存在的空格
        for i in range(len(single_param_def)):
            single_param_def[i] = single_param_def[i].strip()

        if FindMainModuleMode == False:
            # Next find the parameter in the params list and set
            # the corresponding element in paramsvals to the value.
            idx = get_params_idx(glb_param("ignoretype", single_param_def[0], single_param_def[1], "ignoredefval"))
            # If parameter is not found, print useful error message, then exit:
            if idx == -1:
                if filename != "":
                    print("Error: when reading line \"" + line + "\" in parameter file \"" + filename + "\":")
                else:
                    print("Error: when parsing command-line argument \"" + stripped_line_of_text + "\":")
                print("\t\tcould not find parameter \""+ single_param_def[1] + "\" in \""+single_param_def[0]+"\" module.")
                sys.exit(1)
            # If parameter is found at index idx, set paramsval[idx] to the value specified in the file.
            ### 使用文件中的参数值去赋值，主要目的在于重复性代码的编写？
            partype = glb_params_list[idx].type
            if partype == "bool":
                if single_param_def[2] == "True":
                    glb_paramsvals_list[idx] = True
                elif single_param_def[2] == "False":
                    glb_paramsvals_list[idx] = False
                else:
                    print("Error: \"bool\" type can only take values of \"True\" or \"False\"")
                    sys.exit(1)
            elif partype == "int":
                glb_paramsvals_list[idx] = int(single_param_def[2])
            elif partype == "REAL" or \
                partype == "char" or \
                partype == "char *":
                glb_paramsvals_list[idx] = single_param_def[2]
            else:
                print("Error: type \""+partype+"\" on variable \""+ glb_params_list[idx].parname +"\" is unsupported.")
                print("Supported types include: bool, int, REAL, REALARRAY, char, and char *")
                sys.exit(1)
            ## ---------------------------------------------------------------- ##
        ### 是不是在MainModule中找，且一开始设定是没有找到的情况下，主要目的在于验证NRPy是不是从一开始的时候正常运行了，或者私自修改了什么。
        elif FindMainModuleMode == True and MainModuleFound == False:
            if single_param_def[0] == "NRPy" and single_param_def[1] == "MainModule":
                idx = get_params_idx(glb_param("ignoretype", single_param_def[0], single_param_def[1], "ignoredefval"))
                if idx == -1:
                    print("Critical error: NRPy::MainModule is uninitialized!")
                    sys.exit(1)
                glb_paramsvals_list[idx] = single_param_def[2]
### 一个type & module支持多个names & defaultvals,另外就是初始化glb_Cparam这个列表并且返回基于sympy的符号变量
##  返回对象是sympy支持类型
def Cparameters(type,module,names,defaultvals,assumption="Real"):
    output = []
    # if names is not a list, make it a list, to
    #      simplify the remainder of this routine.
    if not isinstance(names,list):
        names = [names]
    defaultval_list = []
    if not isinstance(defaultvals,list):
        for i in range(len(names)):
            defaultval_list.append(defaultvals)
    ## 注意到这里会对默认值的输入是不是列表做一个判断，在是列表同时维数不相同的基础上报错！
    else:
        # If defaultvals *is* a list, then make sure it has the same number of elements as "names".
        if len(defaultvals) != len(names):
            print("Error in Cparameters(): Was provided a list of variables:\n"+str(names)+"\n")
            print("and a list of their default values:\n"+str(defaultvals)+"\n")
            print("but the lists have different lengths ("+str(len(names))+" != "+str(len(defaultvals))+")\n")
            sys.exit(1)
        defaultval_list = defaultvals
    for i in range(len(names)):
        initialize_Cparam(glb_Cparam(type, module, names[i], defaultval_list[i]))
        if assumption == "Real":
            tmp = sp.Symbol(names[i], real=True) # Assumes all Cparameters are real.
        elif assumption == "RealPositive":
            tmp = sp.Symbol(names[i], real=True, positive=True) # Assumes all Cparameters are real and positive.
        else:
            print("Error: assumption "+str(assumption)+" not supported.")
            sys.exit(1)
        output.append(tmp)
    if len(names) == 1:
        return output[0]
    return output
### 默认目录在主目录下面建立相应的文件
##  遍历glb_Cparams_list检测每个变量类型的情况
def generate_Cparameters_Ccodes(directory="./"):
    # Step 1: Check that Cparams types are supported.
    for i in range(len(glb_Cparams_list)):
        partype = glb_Cparams_list[i].type
        if partype != "bool" and \
           partype != "#define" and \
           partype != "char" and \
           partype != "int" and \
           partype != "REAL":
            print("Error: parameter "+glb_Cparams_list[i].module+"::"+glb_Cparams_list[i].parname+" has unsupported type: \""
                  + glb_Cparams_list[i].type + "\"")
            sys.exit(1)

    # Step 2: Generate C code to declare C paramstruct; 
    #         output to "declare_Cparameters_struct.h"
    with open(os.path.join(directory,"declare_Cparameters_struct.h"), "w") as file:
        file.write("typedef struct __paramstruct__ {\n")
        for i in range(len(glb_Cparams_list)):
            if glb_Cparams_list[i].type != "#define":
                if glb_Cparams_list[i].type == "char":
                    Ctype = "char *"
                else:
                    Ctype = glb_Cparams_list[i].type
                file.write(Ctype + " " + glb_Cparams_list[i].parname + ";\n")
        file.write("} paramstruct;\n")

    # Step 3: Generate C code to set all elements in 
    #         C paramstruct to default values; output to 
    #         "set_Cparameters_default.h"
    with open(os.path.join(directory,"set_Cparameters_default.h"), "w") as file:
        for i in range(len(glb_Cparams_list)):
            if glb_Cparams_list[i].type != "#define":
                Coutput = "params." + glb_Cparams_list[i].parname
                if isinstance(glb_Cparams_list[i].defaultval, (bool,int,float)):
                    Coutput += " = " + str(glb_Cparams_list[i].defaultval).lower() + ";\n"
                elif glb_Cparams_list[i].type == "char" and isinstance(glb_Cparams_list[i].defaultval, (str)):
                    Coutput += " = \"" + str(glb_Cparams_list[i].defaultval).lower() + "\";\n"
                else:
                    Coutput += " = " + str(glb_Cparams_list[i].defaultval) + ";\n"
                file.write(Coutput)

    # Step 4: Generate C code to set C parameter constants 
    #         (i.e., all ints != -12345678 and REALs != 1e300); 
    #         output to filename "set_Cparameters.h" if SIMD_enable==False
    #         or "set_Cparameters-SIMD.h" if SIMD_enable==True
    # Step 4.a: Output non-SIMD version, set_Cparameters.h
    def gen_set_Cparameters(pointerEnable=True):
        returnstring = ""
        for i in range(len(glb_Cparams_list)):
            if glb_Cparams_list[i].type == "char":
                Ctype = "char *"
            else:
                Ctype = glb_Cparams_list[i].type
            
            pointer = "->"
            if pointerEnable==False:
                pointer = "."
                
            if not ((Ctype == "REAL" and glb_Cparams_list[i].defaultval == 1e300) or Ctype == "#define"):
                Coutput = "const "+Ctype+" "+glb_Cparams_list[i].parname+" = "+"params"+pointer+glb_Cparams_list[i].parname + ";\n"
                returnstring += Coutput
        return returnstring
        
    with open(os.path.join(directory,"set_Cparameters.h"), "w") as file:
        file.write(gen_set_Cparameters(pointerEnable=True))
    with open(os.path.join(directory,"set_Cparameters-nopointer.h"), "w") as file:
        file.write(gen_set_Cparameters(pointerEnable=False))

    # Step 4.b: Output SIMD version, set_Cparameters-SIMD.h
    with open(os.path.join(directory,"set_Cparameters-SIMD.h"), "w") as file:
        for i in range(len(glb_Cparams_list)):
            if glb_Cparams_list[i].type == "char":
                Ctype = "char *"
            else:
                Ctype = glb_Cparams_list[i].type

            parname = glb_Cparams_list[i].parname
            if Ctype == "REAL" and glb_Cparams_list[i].defaultval != 1e300:
                Coutput =  "const REAL            NOSIMD" + parname + " = " + "params->" + glb_Cparams_list[i].parname + ";\n"
                Coutput += "const REAL_SIMD_ARRAY " + parname + " = ConstSIMD(NOSIMD" + parname + ");\n"
                file.write(Coutput)
            elif glb_Cparams_list[i].defaultval != 1e300 and Ctype !="#define":
                Coutput = "const "+Ctype+" "+parname + " = " + "params->" + glb_Cparams_list[i].parname + ";\n"
                file.write(Coutput)