use rwapadve_ql_software;

select l.code as wikilang, c.title as wikipage, c.content as wikitext
from wc_content c, wc_lang l
where l.id = c.lang_fk
order by l.code, c.title, c.id\G

