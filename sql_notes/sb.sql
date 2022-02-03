
ssn > name
zip > state
ssn, zip, name > street
ssn, zip, name > city


select c.cid from customer c
where NOT EXISTS (select o.oid from order o, pay p
              where o.oid = p.order
              and p.cid = c.cid)
