
Get-ChildItem -Path Cert:\CurrentUser\My
Get-ChildItem -Path Cert:\CurrentUser\Root

$subjectName = "CN=spider"

$cert = Get-ChildItem -Path Cert:\CurrentUser\My | Where-Object { $_.Subject -eq $subjectName }
if($cert)
{
    Remove-Item -Path $cert.PSPath -Force
}

$cert = Get-ChildItem -Path Cert:\CurrentUser\Root | Where-Object { $_.Subject -eq $subjectName }
if($cert)
{
    Remove-Item -Path $cert.PSPath -Force
}

Get-ChildItem -Path Cert:\CurrentUser\My
Get-ChildItem -Path Cert:\CurrentUser\Root
