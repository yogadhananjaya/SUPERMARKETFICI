#include "globals.h"
#include <stdio.h>
#include <string.h>


Supplier dbSupplier[MAX_DATA];
int totalSupplier = 0;

Produk dbProduk[MAX_DATA];
int totalProduk = 0;

Karyawan dbKaryawan[MAX_DATA];
int totalKaryawan = 0;

Gudang dbGudang[MAX_DATA];
int totalGudang = 0;

int screenWidth = 0;
int screenHeight = 0;
int pageOffset = 0;

void updateScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}


void generateDummyData() {
    totalSupplier = 0;
    totalProduk = 0;
    totalKaryawan = 0;
    totalGudang = 0;

    struct RawKaryawan {
        int id; char *n; char *j; char *k; char *u; char *p;
    } rawKaryawan[] = {
        {1, "Budi Harsono", "Store Manager", "081211110001", "b.harsono", "StoreMgr#01"},
        {2, "Siska Amelia", "Asisten Manager", "081211110002", "siska.amel", "Asisten!24"},
        {3, "Dedi Kusuma", "Kepala Gudang", "081211110003", "dedikusuma", "WhHead_03"},
        {4, "Rina Wati", "Kepala Kasir", "081211110004", "rinawati", "ChiefCashier"},
        {5, "Ani Suryani", "Kasir", "081211110005", "anisuryani", "Kasir01!"},
        {6, "Bella Saphira", "Kasir", "081211110006", "bellasaph", "Kasir02!"},
        {7, "Cindy Claudia", "Kasir", "081211110007", "cindyclau", "Kasir03!"},
        {8, "Dina Lorenza", "Kasir", "081211110008", "dinaloren", "Kasir04!"},
        {9, "Eka Sapta", "Kasir", "081211110009", "ekasapta", "Kasir05!"},
        {10, "Fanny Bauty", "Kasir", "081211110010", "fannybau", "Kasir06!"},
        {11, "Gita Gutawa", "Kasir", "081211110011", "gitaguta", "Kasir07!"},
        {12, "Hesty Klepek", "Kasir", "081211110012", "hestyklep", "Kasir08!"},
        {13, "Indah Kalalo", "Kasir", "081211110013", "indahkal", "Kasir09!"},
        {14, "Jihan Fahira", "Kasir", "081211110014", "jihanfah", "Kasir10!"},
        {15, "Kiki Fatmala", "Kasir", "081211110015", "kikifat", "Kasir11!"},
        {16, "Lulu Tobing", "Kasir", "081211110016", "lulutob", "Kasir12!"},
        {17, "Agus Ringgo", "Pramuniaga Food", "081211110017", "agusringgo", "PramuFood1"},
        {18, "Baim Wong", "Pramuniaga Food", "081211110018", "baimwong", "PramuFood2"},
        {19, "Cakra Khan", "Pramuniaga NF", "081211110019", "cakrakhan", "PramuNF1"},
        {20, "Desta Mahendra", "Pramuniaga NF", "081211110020", "desta80s", "PramuNF2"},
        {21, "Edi Brokoli", "Pramuniaga Fresh", "081211110021", "edibrokoli", "PramuFresh1"},
        {22, "Ferry Maryadi", "Pramuniaga Dairy", "081211110022", "ferrymar", "PramuDairy"},
        {23, "Gilang Dirga", "Staff Butcher", "081211110023", "gilangd", "Butcher01"},
        {24, "Hendra Setiawan", "Staff Butcher", "081211110024", "hendraset", "Butcher02"},
        {25, "Irfan Hakim", "Staff Produce", "081211110025", "irfanhakim", "Produce01"},
        {26, "Judika Sihotang", "Staff Receiving", "081211110026", "judikas", "Receiving1"},
        {27, "Kaka Slank", "Staff Receiving", "081211110027", "kakaslank", "Receiving2"},
        {28, "Lilo KLa", "Staff Gudang", "081211110028", "lilokla", "BackStore1"},
        {29, "Melly Goeslaw", "Admin Toko", "081211110029", "mellygoes", "AdminStore"},
        {30, "Nia Ramadhani", "CS Info", "081211110030", "niaramad", "CS_Info"},
        {31, "Olla Ramlan", "CS Member", "081211110031", "ollaram", "CS_Member"},
        {32, "Parto Patrio", "Security", "081211110032", "partopat", "Satpam01"},
        {33, "Qomar Empat", "Security", "081211110033", "qomaremp", "Satpam02"},
        {34, "Rian Dmasiv", "Security", "081211110034", "riandmasiv", "Satpam03"},
        {35, "Sule Sutisna", "Driver Delivery", "081211110035", "sulesutis", "Driver01"},
        {36, "Tora Sudiro", "Driver Delivery", "081211110036", "torasudi", "Driver02"},
        {37, "Uus Komika", "Cleaning Service", "081211110037", "uuskomika", "Cleaner01"},
        {38, "Vino Bastian", "Cleaning Service", "081211110038", "vinobas", "Cleaner02"},
        {39, "Wendy Armoko", "Teknisi Maint", "081211110039", "wendyarm", "TeknisiAC"},
        {40, "Xaverius Wiwid", "Staff Bakery", "081211110040", "xaverius", "RotiFresh"},
        {41, "Yuni Shara", "Staff Bakery", "081211110041", "yunishara", "RotiManis"},
        {42, "Zaskia Adya", "Kasir", "081211110042", "zaskiaad", "Kasir13!"},
        {43, "Andre Taulany", "Supervisor Floor", "081211110043", "andretaul", "SpvFloor"},
        {44, "Billy Syahputra", "Pramuniaga Elek", "081211110044", "billysyah", "ElekStaff"},
        {45, "Chika Jessica", "Pramuniaga Kosme", "081211110045", "chikajess", "BeautyAdv"},
        {46, "Denny Cagur", "Staff Retur", "081211110046", "dennycagur", "ReturBrg"},
        {47, "Eko Patrio", "Spv Security", "081211110047", "ekopatrio", "ChiefSec"},
        {48, "Fatin Shidqia", "Kasir Khusus", "081211110048", "fatinsh", "KasirVIP"},
        {49, "Ge Pamungkas", "Helper Angkut", "081211110049", "gepamung", "Helper01"},
        {50, "Hesti Purwadinata", "Admin Keuangan", "081211110050", "hestipur", "FinanceStore"}
    };

    int countKaryawan = sizeof(rawKaryawan) / sizeof(rawKaryawan[0]);
    for(int i=0; i<countKaryawan; i++) {
        dbKaryawan[i].id = rawKaryawan[i].id;
        strncpy(dbKaryawan[i].nama, rawKaryawan[i].n, 49);
        strncpy(dbKaryawan[i].jabatan, rawKaryawan[i].j, 29);
        strncpy(dbKaryawan[i].kontak, rawKaryawan[i].k, 19);
        strncpy(dbKaryawan[i].username, rawKaryawan[i].u, 29);
        strncpy(dbKaryawan[i].password, rawKaryawan[i].p, 29);
        totalKaryawan++;
    }

    struct RawSupplier {
        int id; char *n; char *a; char *k;
    } rawSupplier[] = {
        {1, "PT Unilever Indonesia Tbk", "BSD City Tangerang", "(021) 808-2001"},
        {2, "PT Indofood Sukses Makmur", "Sudirman Plaza Jakarta", "(021) 579-2002"},
        {3, "PT Mayora Indah Tbk", "Gedung Mayora Jakarta Barat", "(021) 565-2003"},
        {4, "PT Wings Surya (Wings Food)", "Jl. Tipar Cakung Jakarta", "(021) 460-2004"},
        {5, "PT Sinar Sosro", "Gedung Graha Sosro Bekasi", "(021) 884-2005"},
        {6, "PT Coca-Cola Europacific", "Cibitung Bekasi", "(021) 883-2006"},
        {7, "PT Danone Dairy Indonesia", "Cyber 2 Tower Jakarta", "(021) 299-2007"},
        {8, "PT Ultrajaya Milk Industry", "Padalarang Bandung", "(022) 867-2008"},
        {9, "PT Nestle Indonesia", "Perkantoran Hijau Arkadia", "(021) 788-2009"},
        {10, "PT Frisian Flag Indonesia", "Jl. Raya Bogor KM 26", "(021) 871-2010"},
        {11, "PT Santos Jaya Abadi", "Sepanjang Sidoarjo", "(031) 788-2011"},
        {12, "PT Heinz ABC Indonesia", "Daan Mogot Jakarta Barat", "(021) 566-2012"},
        {13, "PT Ajinomoto Indonesia", "Sunter Jakarta Utara", "(021) 653-2013"},
        {14, "PT Kino Indonesia Tbk", "SCBD Jakarta Selatan", "(021) 515-2014"},
        {15, "UD Sayur Lembang Segar", "Lembang Bandung Barat", "(022) 278-2015"},
        {16, "CV Buah Tropis Nusantara", "Pasar Induk Kramat Jati", "(021) 809-2016"},
        {17, "PT Japfa Comfeed", "Wisma Millenia Jakarta", "(021) 831-2017"},
        {18, "PT Charoen Pokphand", "Ancol Jakarta Utara", "(021) 645-2018"},
        {19, "UD Telur Ayam Blitar", "Srengat Blitar Jawa Timur", "(0342) 555-2019"},
        {20, "CV Beras Cianjur Asli", "Cianjur Jawa Barat", "(0263) 264-2020"},
        {21, "PT Garudafood Putra Putri", "Bintaro Tangerang Selatan", "(021) 729-2021"},
        {22, "PT Dua Kelinci", "Pati Jawa Tengah", "(0295) 381-2022"},
        {23, "PT Nippon Indosari", "Kawasan Industri MM2100", "(021) 898-2023"},
        {24, "PT Diamond Cold Storage", "Ancol Barat Jakarta", "(021) 691-2024"},
        {25, "PT Sukanda Djaya (Frozen)", "Kawasan Industri Cibitung", "(021) 883-2025"},
        {26, "PT Reckitt Benckiser", "World Trade Center Jkt", "(021) 252-2026"},
        {27, "PT Unicharm Indonesia", "Karawang International Ind", "(0267) 845-2027"},
        {28, "PT Softex Indonesia", "Tangerang Banten", "(021) 552-2028"},
        {29, "PT Tempo Scan Pacific", "Tempo Scan Tower Jkt", "(021) 292-2029"},
        {30, "PT Kalbe Farma Tbk", "Cempaka Putih Jakarta", "(021) 428-2030"},
        {31, "UD Ikan Laut Muara Angke", "Pelabuhan Muara Angke", "(021) 661-2031"},
        {32, "CV Tahu Tempe Mandiri", "Semanan Jakarta Barat", "(021) 541-2032"},
        {33, "PT Smart Tbk (Filma)", "Thamrin Jakarta Pusat", "(021) 318-2033"},
        {34, "PT Salim Ivomas (Bimoli)", "Sudirman Jakarta", "(021) 579-2034"},
        {35, "PT Nutrifood Indonesia", "Kawasan Industri Pulogadung", "(021) 460-2035"},
        {36, "PT Forisa Nusapersada", "Kawasan Industri Cikupa", "(021) 596-2036"},
        {37, "CV Aneka Kerupuk", "Sidoarjo Jawa Timur", "(031) 896-2037"},
        {38, "UD Bawang Brebes", "Brebes Jawa Tengah", "(0283) 671-2038"},
        {39, "PT Greenfields Indonesia", "Malang Jawa Timur", "(0341) 555-2039"},
        {40, "PT Cimory (Cisarua Dairy)", "Sentul Bogor", "(021) 583-2040"},
        {41, "CV Herbal Nusantara", "Tawangmangu Solo", "(0271) 662-2041"},
        {42, "PT Lion Wings (Ciptadent)", "Cakung Jakarta Timur", "(021) 460-2042"},
        {43, "PT Johnson & Johnson Indo", "Mampang Prapatan Jkt", "(021) 791-2043"},
        {44, "PT Paragon Tech (Wardah)", "Ulujami Jakarta Selatan", "(021) 735-2044"},
        {45, "CV Plastik Kresek Jaya", "Kapuk Jakarta Utara", "(021) 555-2045"},
        {46, "PT Kao Indonesia", "Cikarang Barat Bekasi", "(021) 893-2046"},
        {47, "PT Arnott's Indonesia", "Kawasan Industri Wahana", "(021) 890-2047"},
        {48, "PT Mondelez Indonesia", "Graha Inti Fauzi Jkt", "(021) 791-2048"},
        {49, "UD Gula Merah Nira", "Banyumas Jawa Tengah", "(0281) 635-2049"},
        {50, "CV Supplier Gas LPG", "Tanjung Priok Jakarta", "(021) 430-2050"}
    };

    int countSupplier = sizeof(rawSupplier) / sizeof(rawSupplier[0]);
    for(int i=0; i<countSupplier; i++) {
        dbSupplier[i].id = rawSupplier[i].id;
        strncpy(dbSupplier[i].nama, rawSupplier[i].n, 49);
        strncpy(dbSupplier[i].alamat, rawSupplier[i].a, 99);
        strncpy(dbSupplier[i].kontak, rawSupplier[i].k, 19);
        totalSupplier++;
    }

    struct RawGudang {
        int id; char *n; char *a;
    } rawGudang[] = {
        {1, "DC Cikarang", "Kawasan Jababeka II Blok C"},
        {2, "DC Cibitung", "Kawasan MM2100 (Fresh)"},
        {3, "DC Surabaya", "Kawasan Rungkut Industri Raya"},
        {4, "DC Medan", "Kawasan Industri Medan Star"},
        {5, "DC Makassar", "Kawasan Pergudangan Parangloe"},
        {6, "Gudang Transit Jkt Utara", "Jl. Yos Sudarso Sunter"},
        {7, "Gudang Transit Semarang", "Kawasan Candi Gatot Subroto"},
        {8, "Gudang Transit Palembang", "Jl. Soekarno Hatta Alang-Alang"},
        {9, "Gudang Toko Kemang", "Jl. Kemang Raya No. 10 Jakarta"},
        {10, "Gudang Toko Pondok Indah", "Jl. Metro Pondok Indah Blok 3"},
        {11, "Gudang Toko Kelapa Gading", "Jl. Boulevard Raya Blok M"},
        {12, "Gudang Toko Bintaro", "Jl. Bintaro Utama Sektor 9"},
        {13, "Gudang Toko BSD", "Jl. Pahlawan Seribu Serpong"},
        {14, "Gudang Toko Depok", "Jl. Margonda Raya No. 400"},
        {15, "Gudang Toko Bogor", "Jl. Pajajaran No. 20"},
        {16, "Gudang Toko Bekasi", "Jl. Ahmad Yani (Summarecon)"},
        {17, "Gudang Toko Cibubur", "Jl. Alternatif Cibubur KM 6"},
        {18, "Gudang Toko Bandung Indah", "Jl. Soekarno Hatta Bandung"},
        {19, "Gudang Toko Dago", "Jl. Ir H Djuanda Bandung"},
        {20, "Gudang Toko Semarang Kota", "Jl. Pemuda Semarang"},
        {21, "Gudang Toko Solo", "Jl. Slamet Riyadi Solo"},
        {22, "Gudang Toko Yogya Utara", "Jl. Magelang KM 5 Yogyakarta"},
        {23, "Gudang Toko Surabaya Brt", "Jl. HR Muhammad Surabaya"},
        {24, "Gudang Toko Surabaya Tmr", "Jl. Kertajaya Indah"},
        {25, "Gudang Toko Malang", "Jl. Ijen Boulevard Malang"},
        {26, "Gudang Toko Denpasar", "Jl. Teuku Umar Bali"},
        {27, "Gudang Toko Kuta", "Jl. Bypass Ngurah Rai Bali"},
        {28, "Gudang Toko Mataram", "Jl. Pejanggik Lombok"},
        {29, "Gudang Toko Balikpapan", "Jl. Jend Sudirman Klandasan"},
        {30, "Gudang Toko Samarinda", "Jl. Mulawarman Samarinda"},
        {31, "Gudang Toko Banjarmasin", "Jl. A. Yani KM 5 Kalsel"},
        {32, "Gudang Toko Pontianak", "Jl. Gajah Mada Pontianak"},
        {33, "Gudang Toko Manado", "Jl. Piere Tendean Boulevard"},
        {34, "Gudang Toko Pekanbaru", "Jl. Jend Sudirman Riau"},
        {35, "Gudang Toko Padang", "Jl. Khatib Sulaiman Padang"},
        {36, "Gudang Toko Lampung", "Jl. Raden Intan Bandar Lampung"},
        {37, "Gudang Retur Nasional", "Kawasan Industri Pulo Gadung"},
        {38, "Cold Storage Central Jkt", "Jl. Ancol Barat VIII"},
        {39, "Gudang Barang Promosi", "Jl. Raya Bekasi KM 25"},
        {40, "Gudang Aset & Inventaris", "Jl. TB Simatupang Cilandak"},
        {41, "Gudang Arsip Dokumen", "Jl. Raya Bogor KM 30"},
        {42, "Gudang E-Commerce", "Jl. Halim Perdanakusuma"},
        {43, "Cross-Docking Serang", "Jl. Raya Serang-Cilegon"},
        {44, "Cross-Docking Cirebon", "Jl. Bypass Cirebon"},
        {45, "Cross-Docking Tegal", "Jl. Pantura Tegal"},
        {46, "Hub Logistik Banyuwangi", "Jl. Gatot Subroto Banyuwangi"},
        {47, "Gudang Toko Jambi", "Jl. Gatot Subroto Pasar Jambi"},
        {48, "Gudang Toko Batam", "Nagoya Hill Area Batam"},
        {49, "Gudang Toko Jayapura", "Jl. Raya Sentani Papua"},
        {50, "Gudang Toko Sorong", "Jl. Jend Sudirman Sorong"}
    };

    int countGudang = sizeof(rawGudang) / sizeof(rawGudang[0]);
    for(int i=0; i<countGudang; i++) {
        dbGudang[i].id = rawGudang[i].id;
        strncpy(dbGudang[i].nama, rawGudang[i].n, 49);
        strncpy(dbGudang[i].alamat, rawGudang[i].a, 99);
        totalGudang++;
    }

    struct RawProduk {
        int id; char *n; int s; long h;
    } rawProduk[] = {
        {1, "Beras Pandan Wangi 5kg", 150, 68000},
        {2, "Minyak Goreng Bimoli 2L", 200, 42500},
        {3, "Gula Pasir Gulaku 1kg", 300, 16500},
        {4, "Telur Ayam Negeri Tray", 100, 28000},
        {5, "Indomie Goreng Special Dus", 250, 118000},
        {6, "Susu UHT Ultra Milk 1L", 180, 19500},
        {7, "Kecap Bango 550ml Refill", 150, 23000},
        {8, "Saus Sambal ABC Extra 335ml", 120, 15000},
        {9, "Teh Celup Sariwangi 25s", 200, 6500},
        {10, "Kopi Kapal Api Mix 10s", 250, 14500},
        {11, "Tepung Segitiga Biru 1kg", 150, 12000},
        {12, "Garam Meja Refina 500g", 100, 4500},
        {13, "Santan Kara 200ml", 300, 9500},
        {14, "Margarin Blue Band 200g", 180, 10500},
        {15, "Keju Kraft Cheddar 165g", 120, 24000},
        {16, "Cokelat Silverqueen 65g", 150, 16500},
        {17, "Biskuit Roma Kelapa 300g", 140, 11500},
        {18, "Chitato Sapi Panggang 68g", 160, 10500},
        {19, "Air Mineral Aqua 1500ml", 500, 6000},
        {20, "Coca Cola 1.5 Liter", 120, 15000},
        {21, "Jus Buavita Jambu 1L", 80, 26000},
        {22, "Cimory Squeeze Blueberry", 150, 11000},
        {23, "Apel Fuji RRC (per Kg)", 50, 35000},
        {24, "Jeruk Medan (per Kg)", 60, 28000},
        {25, "Pisang Cavendish Sunpride", 40, 22000},
        {26, "Daging Sapi Rendang 500g", 30, 65000},
        {27, "Ayam Broiler Utuh 0.9kg", 45, 38000},
        {28, "Ikan Salmon Fillet 200g", 20, 75000},
        {29, "Nugget Fiesta Chicken 500g", 80, 48000},
        {30, "Sosis Kanzler Beef 500g", 70, 52000},
        {31, "Sabun Lifebuoy Cair 450ml", 150, 24500},
        {32, "Shampoo Pantene 290ml", 120, 38000},
        {33, "Pepsodent Action 123 190g", 180, 14500},
        {34, "Sikat Gigi Ciptadent Isi 3", 100, 12000},
        {35, "Deterjen Rinso Bubuk 800g", 150, 26000},
        {36, "Molto Sachet Renceng", 200, 12000},
        {37, "Sunlight 755ml", 180, 18500},
        {38, "Super Pell 770ml", 140, 16000},
        {39, "Tissue Paseo Smart 250s", 200, 11000},
        {40, "Tissue Toilet Livi Roll", 300, 4500},
        {41, "Mamypoko Pants L30", 60, 58000},
        {42, "Pembalut Laurier Day 20s", 120, 15000},
        {43, "Hit Aerosol 600ml", 80, 35000},
        {44, "Baterai Alkaline AA isi 2", 100, 18000},
        {45, "Lampu LED Philips 9 Watt", 50, 42000},
        {46, "Spon Scotch Brite", 150, 6500},
        {47, "Kantong Sampah Hitam Pack", 100, 15000},
        {48, "Roti Tawar Sari Roti", 60, 16500},
        {49, "Selai Strawberry Morin", 80, 22000},
        {50, "Madu TJ Murni 150g", 90, 24000}
    };

    int countProduk = sizeof(rawProduk) / sizeof(rawProduk[0]);
    for(int i=0; i<countProduk; i++) {
        dbProduk[i].id = rawProduk[i].id;
        strncpy(dbProduk[i].nama, rawProduk[i].n, 49);
        dbProduk[i].stok = rawProduk[i].s;
        dbProduk[i].harga = rawProduk[i].h;
        totalProduk++;
    }
}