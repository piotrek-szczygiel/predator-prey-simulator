if ($args.Count -ne 2)
{
    Write-Host "usage: $PSCommandPath <version> <executable path>"
    exit 1
}

$Version = $args[0]
$Executable = $args[1]

$ReleaseDir = "release/sim-$Version"
$ReleaseZip = "release/sim-$Version.zip"

Remove-Item -LiteralPath "$ReleaseDir" -Force -Recurse -ErrorAction Ignore
Remove-Item -LiteralPath "$ReleaseZip" -Force -ErrorAction Ignore

New-Item -ItemType Directory -Path "$ReleaseDir" | Out-Null

Copy-Item "assets" -Destination "$ReleaseDir" -Recurse
Copy-Item "config.ini" -Destination "$ReleaseDir"

$Log = "$env:TEMP/rh.log"
$LogUTF8 = "$Log.utf8"
ResourceHacker.exe -open $Executable -save "$ReleaseDir/sim.exe" -action addskip -res "assets/icon.ico" -mask "ICONGROUP,MAINICON," -log $Log | Out-Null

Get-Content $Log -Encoding Unicode | Set-Content $LogUTF8 -Encoding Utf8

if (Select-String -Path $LogUTF8 -Pattern "Failed!" -Quiet)
{
    Get-Content $LogUTF8
    Write-Host "Error while running ResourceHacker!" -ForegroundColor Red
    exit 2
}

Compress-Archive -Path $ReleaseDir -DestinationPath $ReleaseZip
Remove-Item -LiteralPath "$ReleaseDir" -Force -Recurse -ErrorAction Ignore

Write-Host "Release successfully packed to $ReleaseZip" -ForegroundColor Green
