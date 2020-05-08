"""
Demonstrate execution of an arbitrary sql that returns output parameters

Output paramenters must be defined before using.
Parameter definition is done with cursor.var(cx_Oracle.<datatype>)
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""
    begin
        select :i_intParam * 2, :i_strParam||:i_strParam into :o_intParam, :o_strParam from dual;
    end;
"""

# define output parameters that will be passed to sql statement
outputIntParam = cursor.var(cx_Oracle.NUMBER)
outputStrParam = cursor.var(cx_Oracle.STRING)

params = {
    'i_intParam':10,
    'i_strParam':'ABC',
    'o_intParam':outputIntParam,
    'o_strParam':outputStrParam
}

cursor.execute(sql, params)

print("outputIntParam = %d" % outputIntParam.getvalue())
print("outputStrParam = %s" % outputStrParam.getvalue())
