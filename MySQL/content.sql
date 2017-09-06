use rwapadve_ql_software;

-- If there's an alternative title for a page, use it,
-- Otherwise, just use the main title.
select  l.code as wikilang, 
        case c.alternate_title when ""
            then c.title
            else c.alternate_title
        end as wikipage,
        c.content as wikitext
from    wc_content c, wc_lang l
where   l.id = c.lang_fk
order   by l.code, c.title, c.id\G
