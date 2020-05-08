"""
Demonstrate table updating using input parameter

Before running this demo, run demo 04_insert.py one or more times, to create some records in the table
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""
begin
    update REQUESTS set DETAILS = :i_newDetails where DETAILS = :i_oldDetails;
    commit;
end;
"""
params = {
    ':i_oldDetails' : 'Sample request text',
    ':i_newDetails' : 'Updated request text'
    }
cursor.execute(sql, params)

connection.close()
print("Now see REQUESTS table content to check that records are updated")
