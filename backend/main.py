from fastapi import FastAPI
from sqlalchemy import create_engine, text
import os

app = FastAPI(title="CapAnalysis API", version="2.0.0")

DB_URL = os.getenv("DATABASE_URL", "postgresql://capana:password@localhost/capanalysis")
engine = create_engine(DB_URL)

@app.get("/")
def read_root():
    return {"status": "ok", "service": "CapAnalysis API v2"}

@app.get("/system/status")
def system_status():
    try:
        with engine.connect() as conn:
            result = conn.execute(text("SELECT version();")).fetchone()
            return {"db_status": "connected", "db_version": result[0]}
    except Exception as e:
        return {"db_status": "error", "error": str(e)}
