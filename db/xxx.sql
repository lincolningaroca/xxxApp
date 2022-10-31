BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "urls" (
	"id"	integer NOT NULL,
	"url"	text NOT NULL,
	"desc"	text DEFAULT 'no desc',
	"category_id"	integer,
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("category_id") REFERENCES "category"("id")
);
CREATE TABLE IF NOT EXISTS "category" (
	"id"	integer NOT NULL,
	"category_name"	text NOT NULL,
	"desc"	text,
	PRIMARY KEY("id" AUTOINCREMENT)
);
INSERT INTO "urls" VALUES (1,'https://www.xvideos.com/video66477349/si_mi_vagina_sigue_tragando_semen_me_voy_a_quedar_embarazada','Si mi vagina sigue tragando semen me voy a quedar embarazada.',1);
INSERT INTO "urls" VALUES (2,'https://www.xvideos.com/video68434603/madura_caliente','Madura caliente.',1);
INSERT INTO "urls" VALUES (3,'https://www.xvideos.com/video12771995/culeando_con_mi_amiga_en_hilo_y_me_vengo_dentro_de_su_chucha_-_ricos_sentones
','Culeando con mi amiga en Hilo y me vengo dentro de su chucha - ricos sentones.',1);
INSERT INTO "urls" VALUES (4,'https://www.xvideos.com/video65381917/la_monta_bien_rico
','La Monta Bien Rico.',1);
INSERT INTO "urls" VALUES (5,'https://www.xvideos.com/video25825429/que_rico_se_mueve_mi_nena
','Que rico se mueve mi nena.',1);
INSERT INTO "urls" VALUES (6,'https://www.xvideos.com/video58907723/que_rico_es_cojer_con_otro_hombre_que_no_sea_tu_esposo','Que rico es cojer con otro hombre que no sea tu esposo.',1);
INSERT INTO "urls" VALUES (9,'https://www.xnxx.com/video-15gd4paa/la_mejor_amiga_de_mi_novia_queria_que_me_la_cogiera_y_se_la_meti_toda_sin_que_mi_novia_se_enterara','La mejor amiga de mi novia quería que me la cogiera y se la metí toda sin que mi novia se enterará.',1);
INSERT INTO "urls" VALUES (10,'https://www.youtube.com/watch?v=M7K_i2wdBCI','Postgresql: Cifrado de Datos 1 - Hash, Cifrados Básico y Cifrado Simétrico.',10);
INSERT INTO "category" VALUES (1,'VIDEOS PARA ADULTOS','puro videos de porno y categoria solo para adultos.');
INSERT INTO "category" VALUES (10,'VIDEOS DE YOU TUBE DE PROGRAMACIÓN','varios videos de desarrollo de sistemas y base de datos.');
COMMIT;
