
$currentDir = $PWD.Path

$pfxPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_privatekey_and_certificate.pfx"
$pfxPassword = "spider"  # pfx password
$securePassword = ConvertTo-SecureString $pfxPassword -AsPlainText -Force
Import-PfxCertificate -FilePath $pfxPath -CertStoreLocation Cert:\CurrentUser\My -Password $securePassword

$certPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_certificate.cer"
Import-Certificate -FilePath $certPath -CertStoreLocation Cert:\CurrentUser\Root

Get-ChildItem -Path Cert:\CurrentUser\My
Get-ChildItem -Path Cert:\CurrentUser\Root
