from fastapi import FastAPI, Depends, HTTPException
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker, Session
import os
from models import Base, Dataset

app = FastAPI(title="CapAnalysis API", version="2.0.0")

DB_URL = os.getenv("DATABASE_URL", "postgresql://capana:password@localhost/capanalysis")
engine = create_engine(DB_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.get("/")
def read_root():
    return {"status": "ok", "service": "CapAnalysis API v2"}

@app.get("/system/status")
def system_status(db: Session = Depends(get_db)):
    try:
        result = db.execute(text("SELECT version();")).fetchone()
        return {"db_status": "connected", "db_version": result[0]}
    except Exception as e:
        return {"db_status": "error", "error": str(e)}

@app.get("/datasets")
def list_datasets(db: Session = Depends(get_db)):
    datasets = db.query(Dataset).all()
    return datasets
