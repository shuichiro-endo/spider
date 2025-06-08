
$currentDir = $PWD.Path

$certPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_certificate.cer"
Import-Certificate -FilePath $certPath -CertStoreLocation Cert:\CurrentUser\Root

Get-ChildItem -Path Cert:\CurrentUser\Root
