-- problem 1
-- list the completed games that nodbody bet on

(select game from out)
except
(select game from bets)

/*

candidate key uniquely identifies a tuple according to a key constraint
*/


select s.sname
from sailor s, reserves r, boats b
where r.sid = s.sid and b.bid = r.bid and (b.color = 'red' or b.color = 'green')
