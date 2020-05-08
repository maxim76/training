"""
Demonstrate insertion into table using input parameter
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""
begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, :i_details);
    commit;
end;
"""
params = {':i_details' : 'Sample request text'}
cursor.execute(sql, params)

connection.close()
print("Now see REQUESTS table content to check that new record is inserted")
