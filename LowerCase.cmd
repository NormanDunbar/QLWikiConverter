@echo off
REM 
REM Batch file to convert a Wiclear 'Page-Title.en.txt' file to DokuWiki's
REM 'page_title.txt' format, and batch convert all the files passed from the
REM 'Pages' folder to an output in the 'DW' folder.
REM
REM Letters are lower cased, spaces or punctuation are underscores. As we already
REM have the punctuation and spaces change d to hyphens, we simply replace those
REM with underscores.
REM
REM Norman Dunbar
REM 18th August 2017.
REM
REM Mostly stolen from http://www.robvanderwoude.com/battech_convertcase.php - thanks.
REM
SETLOCAL ENABLEDELAYEDEXPANSION
SET FileName=%1
CALL :LowerCase FileName
echo %FileName%
ENDLOCAL
GOTO:EOF

:LowerCase
:: Subroutine to convert a variable VALUE to all lower case.
:: The argument for this subroutine is the variable NAME.
SET %~1=!%1:A=a!
SET %~1=!%1:B=b!
SET %~1=!%1:C=c!
SET %~1=!%1:D=d!
SET %~1=!%1:E=e!
SET %~1=!%1:F=f!
SET %~1=!%1:G=g!
SET %~1=!%1:H=h!
SET %~1=!%1:I=i!
SET %~1=!%1:J=j!
SET %~1=!%1:K=k!
SET %~1=!%1:L=l!
SET %~1=!%1:M=m!
SET %~1=!%1:N=n!
SET %~1=!%1:O=o!
SET %~1=!%1:P=p!
SET %~1=!%1:Q=q!
SET %~1=!%1:R=r!
SET %~1=!%1:S=s!
SET %~1=!%1:T=t!
SET %~1=!%1:U=u!
SET %~1=!%1:V=v!
SET %~1=!%1:W=w!
SET %~1=!%1:X=x!
SET %~1=!%1:Y=y!
SET %~1=!%1:Z=z!
rem
rem And a hyphen becomes an underscore.
rem
SET %~1=!%1:-=_!
GOTO:EOF

