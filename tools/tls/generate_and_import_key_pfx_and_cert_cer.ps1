
$currentDir = $PWD.Path

$cert = New-SelfSignedCertificate -Type "SSLServerAuthentication" -Subject "CN=spider" -CertStoreLocation "cert:\CurrentUser\My" -KeyAlgorithm "ECDSA_nistP256" -KeyExportPolicy Exportable -NotAfter (Get-Date).AddYears(10)

Get-ChildItem -Path Cert:\CurrentUser\My

$pfxPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_privatekey_and_certificate.pfx"
$pfxPassword = "spider"  # pfx password
$cert | Export-PfxCertificate -FilePath $pfxPath -Password (ConvertTo-SecureString -String $pfxPassword -Force -AsPlainText)

$certPath = Join-Path -Path $currentDir -ChildPath "pipe_server_https_certificate.cer"
$cert | Export-Certificate -FilePath $certPath -Type CERT
Import-Certificate -FilePath $certPath -CertStoreLocation Cert:\CurrentUser\Root

Get-ChildItem -Path Cert:\CurrentUser\Root
