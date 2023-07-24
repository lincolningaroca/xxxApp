PRAGMA foreign_keys=ON;
BEGIN TRANSACTION;
CREATE TABLE users(user_id integer primary key autoincrement,
user text not null unique,
password text not null,
user_profile varchar(20) not null,
rescue_type varchar(40) not null,
first_value text not null,
confirm_value text not null);
INSERT INTO users VALUES(1,'public','public','PUBLIC','PUBLIC','PUBLIC','PUBLIC');
INSERT INTO users VALUES(2,'lincoln','lincoln123','USER','PIN','123','123');
INSERT INTO users VALUES(3,'carolina','carolina321','USER','PIN','321','321');
CREATE TABLE urls(url_id integer primary key autoincrement,
url text not null,
desc text,
categoryid integer references category(category_id));
INSERT INTO urls VALUES(1,'www.juegos.com','juegos retro de 1980',1);
INSERT INTO urls VALUES(2,'www.arcade.com','juegos arcade',1);
INSERT INTO urls VALUES(3,'www.juegos_retro80s.com','juegos retro de 1980 a 1990',2);
INSERT INTO urls VALUES(4,'www.arcade/maquinas.com','juegos arcade de maquinas pinball',2);
INSERT INTO urls VALUES(5,'www.ansic_standar.io','LIBROS DE PROGRAMACION MODULAR EN PURO C',3);
INSERT INTO urls VALUES(6,'www.ansic.net','LIBROS DE PROGRAMACION EN C',3);
INSERT INTO urls VALUES(7,'www.youtube.com','videos de desarrollo en C#',4);
INSERT INTO urls VALUES(8,'www.profesorretroman.com','video de desarrollo de videojuegos en c++',4);
INSERT INTO urls VALUES(9,'www.pj/sentencias.gob.pe','evento de juicios y sentencias',5);
INSERT INTO urls VALUES(10,'www.poderjudicial.gob.pe','enventos del poder judicial',5);
INSERT INTO urls VALUES(11,'www.n_legislacion.pe','nueva legislacion de leyes en el peru',6);
INSERT INTO urls VALUES(12,'www.laley2023.gob.pe','presentacion de leyes nuevas',6);
INSERT INTO urls VALUES(15,'www.petardas.com.pet','muchos videos de mierda!!',8);
INSERT INTO urls VALUES(16,'http://www.petardas.pet','VIDEOS PORNO, CASEROS PERUANOS.',10);
INSERT INTO urls VALUES(18,'http://www.laleydelpurblo.com.pe','LEYES PARA DEFENDER AL PUEBLO',6);
INSERT INTO urls VALUES(19,'www.com.pe','DFGHJK',3);
CREATE TABLE category(category_id integer primary key autoincrement,
category_name text not null,
desc text,
userid integer references users(user_id));
INSERT INTO category VALUES(1,'JUEGOS PARA PC','VARIOS JUEGOS PARA PC, Y EMULADORES TAMBIEN',1);
INSERT INTO category VALUES(2,'JUEGOS 2','COLECCION DE JUEGOS RETRO, PARA CONOCEDORES.',1);
INSERT INTO category VALUES(3,'LIBROS DIGITALES','DIVERSOS LIBROS PARA CONSULTAR',2);
INSERT INTO category VALUES(4,'VIDEO TUTORIALES','TUTORIALES VARIOS',2);
INSERT INTO category VALUES(5,'eventos poder judicial','notificaciones de eventos en el poder judicial',3);
INSERT INTO category VALUES(6,'leyes de la legislacion peruana','recursos de leyes ene l peru',3);
INSERT INTO category VALUES(8,'VIDEOS PORNO','CUALQUIER COSA',2);
INSERT INTO category VALUES(10,'VIDEOS PORNO','MUCHO PORNO CESERO',1);
DELETE FROM sqlite_sequence;
INSERT INTO sqlite_sequence VALUES('users',3);
INSERT INTO sqlite_sequence VALUES('category',10);
INSERT INTO sqlite_sequence VALUES('urls',19);
COMMIT;
