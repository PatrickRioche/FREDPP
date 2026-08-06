param(
    [string]$Repository = "PatrickRioche/FREDPP"
)

$ErrorActionPreference = "Stop"
$Headers = @{
    Accept = "application/vnd.github+json"
    "X-GitHub-Api-Version" = "2022-11-28"
}

$Releases = Invoke-RestMethod -Headers $Headers -Uri "https://api.github.com/repos/$Repository/releases?per_page=100"
$Rows = foreach ($Release in $Releases) {
    foreach ($Asset in $Release.assets) {
        [PSCustomObject]@{
            Version = $Release.tag_name
            Fichier = $Asset.name
            Telechargements = [int]$Asset.download_count
        }
    }
}

$Rows | Sort-Object Version, Fichier | Format-Table -AutoSize

$Windows = ($Rows | Where-Object Fichier -Match 'windows-x64\.zip$' | Measure-Object Telechargements -Sum).Sum
$Debian = ($Rows | Where-Object Fichier -Match 'debian13-amd64\.tar\.gz$|_amd64\.deb$' | Measure-Object Telechargements -Sum).Sum
$Total = ($Rows | Measure-Object Telechargements -Sum).Sum

if ($null -eq $Windows) { $Windows = 0 }
if ($null -eq $Debian) { $Debian = 0 }
if ($null -eq $Total) { $Total = 0 }

Write-Host "Windows : $Windows"
Write-Host "Debian  : $Debian"
Write-Host "Total   : $Total"
