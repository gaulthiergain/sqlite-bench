DROP TABLE IF EXISTS [artists_backup];
CREATE TABLE artists_backup(
   artistid INTEGER PRIMARY KEY AUTOINCREMENT,
   name NVARCHAR
);
INSERT INTO artists_backup SELECT artistid, name FROM artist;
SELECT artistid, name FROM artists_backup;

DELETE FROM artists_backup WHERE artistid = 1;
DELETE FROM artists_backup WHERE name LIKE '%Santana%';
DELETE FROM artists_backup;

DROP TABLE IF EXISTS [Album];
DROP TABLE IF EXISTS [Artist];
DROP TABLE IF EXISTS [Customer];
DROP TABLE IF EXISTS [Employee];
DROP TABLE IF EXISTS [Genre];
DROP TABLE IF EXISTS [Invoice];
DROP TABLE IF EXISTS [InvoiceLine];
DROP TABLE IF EXISTS [MediaType];
DROP TABLE IF EXISTS [Playlist];
DROP TABLE IF EXISTS [PlaylistTrack];
DROP TABLE IF EXISTS [Track];
