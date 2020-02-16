import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()

statement = '''
begin
    select :i_intParam * 2, :i_strParam||:i_strParam into :o_intParam, :o_strParam from dual;
end;    
'''

outputIntParam = cursor.var(cx_Oracle.NUMBER)
outputStrParam = cursor.var(cx_Oracle.STRING)

parameters = {
    'i_intParam':10,
    'i_strParam':'ABC',
    'o_intParam':outputIntParam,
    'o_strParam':outputStrParam
}

print("Request parameters")
print(parameters)

print('Executing request... ', end='')
cursor.execute(statement, parameters)
print('Done.')

print('configured binding:')
print(cursor.bindnames())

print("outputIntParam = %d" % outputIntParam.getvalue())
print("outputStrParam = %s" % outputStrParam.getvalue())
