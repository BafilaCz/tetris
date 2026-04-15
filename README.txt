// SPUŠTĚNÍ

Ke spuštění je nutné mít nainstalované určité SDL knihovny. Pro instalaci stačí spustit v terminálu následující příkaz:


------------------------
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
------------------------

Potom lze program spustit pomocí programu cmake. Následujicí příkazy vytvoří složku build a v ní vytvoří spustitelný soubor "tetris".

------------------------
mkdir build
cd build
cmake ..
make
------------------------
po vytvoření stačí spustit program ze složky "build" pomocí příkazu:

------------------------
./tetris
------------------------

// OVLÁDÁNÍ

Po zapnutí se objeví úvodní menu kde je možné nastavit nebo spustit hru.
V nastavení je možné si zvolit jednu ze 3 obtížností (EASY/MEDIUM/HARD). Zvolit si jednu ze 3 písniček na pozadí, a upravit hlasitost celé hry.
Pro navigaci v MENU je možné použít myš nebo klávesu ESC.
Samotná hra se ovládá pomocí šipek nebo WASD a zpět do hlavního menu se jde dostat klávesou ESC.

// HRA

Hra samotná je předělávka klasického tetrisu. Náhodně vygenerované bloky (tetromina) padají automaticky od shora dolů a při kolizi s plným políčkem nebo dolním okrajem mřížky se celý tento blok zaplní místo kde se právě nachází. Vašim úkolem je naskládat tetromina tak, abyste zaplnili celý řádek. Zaplněné řádky se automaticky vymažou a vám se tak uvolní místo a přičte score. Pokud se stane že tetromina není možné umístit hra končí a začínáte od znova.


