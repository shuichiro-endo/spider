
$currentDir = $PWD.Path

$certPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_certificate.pem"
Import-Certificate -FilePath $certPath -CertStoreLocation Cert:\CurrentUser\Root

Get-ChildItem -Path Cert:\CurrentUser\Root
