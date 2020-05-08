----------------------------------------------------
-- Export file for user TEST                      --
-- Created by m_klimovich2 on 8/05/2020, 17:00:38 --
----------------------------------------------------

spool cx_oracle_tutorial_db_objects.log

prompt
prompt Creating table REQUESTS
prompt =======================
prompt
create table REQUESTS
(
  ID      NUMBER not null,
  DT      DATE not null,
  DETAILS VARCHAR2(64) not null
)
;

prompt
prompt Creating sequence SQ$REQUESTS
prompt =============================
prompt
create sequence SQ$REQUESTS
minvalue 0
maxvalue 9999999999999999999999999999
start with 41
increment by 1
cache 20;

prompt
prompt Creating package PS
prompt ===================
prompt
create or replace package PS is

  -- Author  : MAXIMK
  -- Created : 7/05/2020 14:30:38
  -- Purpose : Demostrate Python - Oracle integration

-- Procedure with output parameter. Insert records into request table, new record ID returned as output parameter. Changes are not commited by procedure.
procedure AddRequestProc(i_details in varchar2,  o_ID out integer);

-- Function to Insert records into request table, new record ID returned as function result. Changes are not commited by function.
function  AddRequestFunc(i_details in varchar2) return integer;


-- Procedure to Insert records into request table and commit changes
procedure AddRequestProcWithCommit(i_details in varchar2);

-- Procedure with output parameter. Insert records into request table, new record ID returned as output parameter. Commit changes.
procedure AddRequestProcOutWithCommit(i_details in varchar2, o_ID out integer, o_ResultDescr out varchar2);

-- Function to Insert records into request table, new record ID returned as function result. Commit changes
function  AddRequestFuncWithCommit(i_details in varchar2) return integer;

-- Function with output parameter. Insert records into request table, new record ID returned as function result. Verbal result description returned as output parameter. Commit changes
function  AddRequestFuncOutWithCommit(i_details in varchar2, o_ResultDescr out varchar2) return integer;

end PS;
/

prompt
prompt Creating package body PS
prompt ========================
prompt
create or replace package body PS is

  procedure AddRequestProc(i_details in varchar2, o_ID out integer) is
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details) returning ID into o_ID;
  end;
    
  function AddRequestFunc(i_details in varchar2) return integer is
    NewID integer;
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details) returning ID into NewID;
    return NewID;
  end;

  procedure AddRequestProcWithCommit(i_details in varchar2) is
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details);
    commit;
  end;
  
  procedure AddRequestProcOutWithCommit(i_details in varchar2, o_ID out integer, o_ResultDescr out varchar2) is
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details) returning ID into o_ID;
    o_ResultDescr := 'Request added successfully';
    commit;
  end;
    
  function AddRequestFuncWithCommit(i_details in varchar2) return integer is
    NewID integer;
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details) returning ID into NewID;
    commit;
    return NewID;
  end;

  function AddRequestFuncOutWithCommit(i_details in varchar2, o_ResultDescr out varchar2) return integer is
    NewID integer;
  begin
    insert into REQUESTS (ID, DT, DETAILS) values (SQ$REQUESTS.nextval, sysdate, i_details) returning ID into NewID;
    commit;
    o_ResultDescr := 'Request added successfully';
    return NewID;
  end;

end PS;
/


spool off
