"""
Demonstrate how to prepare sql statement, to make subsequent calls be more efficient.

prepare() can be used before a call to execute() to define the statement that will be executed. 
When this is done, the prepare phase will not be performed when the call to execute() is made with None or the same string object as the statement. 
"""
import cx_Oracle
import dbconfig
from pprint import pprint

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""
    begin
        select :i_intParam * 2, :i_strParam||:i_strParam into :o_intParam, :o_strParam from dual;
    end;
"""
cursor.prepare(sql)

print('Check the list of bind variable names bound to the statement:')
pprint(cursor.bindnames())

# define output parameters that will be passed to sql statement
outputIntParam = cursor.var(cx_Oracle.NUMBER)
outputStrParam = cursor.var(cx_Oracle.STRING)

params = {
    'i_intParam':10,
    'i_strParam':'ABC',
    'o_intParam':outputIntParam,
    'o_strParam':outputStrParam
}

# execute() can be called with None statemtn, if the cursor is already prepared to work with some statement
cursor.execute(None, params)

print("outputIntParam = %d" % outputIntParam.getvalue())
print("outputStrParam = %s" % outputStrParam.getvalue())
